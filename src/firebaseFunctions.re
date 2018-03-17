module Https = {
  module Response = {
    type t;
    type cookieOptions = {
      .
      "domain": Js.Nullable.t(string),
      "encode": Js.Nullable.t(string => string),
      "expires": Js.Nullable.t(float),
      "httpOnly": Js.Nullable.t(bool),
      "maxAge": int,
      "path": Js.Nullable.t(string),
      "secure": Js.Nullable.t(bool),
      "signed": Js.Nullable.t(bool),
    };
    let oneYear = 1000 * 60 * 60 * 24 * 365;
    let createCookieOptions =
        (
          ~domain: option(string)=?,
          ~encode: option(string => string)=?,
          ~expires: option(float)=?,
          ~httpOnly: option(bool)=?,
          ~maxAge: int=oneYear,
          ~path: option(string)=?,
          ~secure: option(bool)=?,
          ~signed: option(bool)=?,
          (),
        ) => {
      "domain": Js.Nullable.fromOption(domain),
      "encode": Js.Nullable.fromOption(encode),
      "expires": Js.Nullable.fromOption(expires),
      "httpOnly": Js.Nullable.fromOption(httpOnly),
      "maxAge": maxAge,
      "path": Js.Nullable.fromOption(path),
      "secure": Js.Nullable.fromOption(secure),
      "signed": Js.Nullable.fromOption(signed),
    };
    [@bs.send.pipe: t] external json : Js.t('a) => t = "";
    [@bs.send.pipe: t] external status : int => t = "";
    [@bs.send.pipe: t] external redirect : string => t = "";
    [@bs.send.pipe: t]
    external cookie : (string, string, cookieOptions) => t = "";
  };
  module Request = {
    type t;
    [@bs.get] external body : t => Js.Json.t = "";
    [@bs.get] external query : t => Js.t('a) = "";
    [@bs.get] external cookies : t => Js.t('a) = "";
    [@bs.get] external headers : t => Js.Json.t = "";
  };
  type t;
  type requestHandler = (Request.t, Response.t, unit => unit) => unit;
  [@bs.scope "https"] [@bs.module "firebase-functions"]
  external onRequest : requestHandler => t = "";
  module CookieParser = {
    [@bs.module "cookie-parser"]
    external make : unit => requestHandler = "default";
    [@bs.module "cookie-parser"]
    external makeWithSecret : string => requestHandler = "default";
    [@bs.module "cookie-parser"]
    external makeWithOptions : Js.t('a) => requestHandler = "default";
    [@bs.module "cookie-parser"]
    external makeWithSecretAndOptions : (string, Js.t('a)) => requestHandler =
      "default";
  };
  module ComposeMiddleware = {
    [@bs.module "compose-middleware"]
    external make : array(requestHandler) => requestHandler = "compose";
  };
};

module PubSub = {
  type t;
  module rec Topic: {
    type t;
    [@bs.send.pipe: t]
    external onPublish : (Event.t => Js.Promise.t('a)) => t = "";
  } = Topic
  and Message: {
    type t;
    [@bs.get] external attributes : t => Js.t('a) = "";
    [@bs.get] external data : t => string = "";
    [@bs.get] external json : t => 'a = "";
    [@bs.send] external toJSON : t => Js.Json.t = "";
  } = Message
  and Event: {
    type t;
    [@bs.get] external data : t => Message.t = "";
    [@bs.get] external params : t => Js.t('a) = "";
  } = Event;
  [@bs.scope "pubsub"] [@bs.module "firebase-functions"]
  external topic : string => Topic.t = "";
};

module Config = {
  type t;
  [@bs.module "firebase-functions"] external make : unit => t = "config";
  [@bs.get] external firebase : t => Js.t('a) = "";
};

module Error = {
  type t;
};

module Database = {
  type t;
  module rec RefBuilder: {
    type t;
    [@bs.send.pipe: t]
    external onWrite : (Event.t => Js.Promise.t('a)) => t = "";
    [@bs.send.pipe: t]
    external onCreate : (Event.t => Js.Promise.t('a)) => t = "";
    [@bs.send.pipe: t]
    external onDelete : (Event.t => Js.Promise.t('a)) => t = "";
    [@bs.send.pipe: t]
    external onUpdate : (Event.t => Js.Promise.t('a)) => t = "";
  } = RefBuilder
  and DeltaSnapshot: {
    type t;
    [@bs.get] external key : t => Js.nullable(string) = "";
    [@bs.get] external ref : t => FirebaseAdmin.Database.Reference.t = "";
    [@bs.get] external adminRef : t => FirebaseAdmin.Database.Reference.t = "";
    [@bs.get] external previous : t => t = "";
    [@bs.send] external child : (t, ~path: string) => t = "";
    [@bs.send] external exists : t => bool = "";
    [@bs.send] external exportVal : t => Js.Json.t = "exportVal";
    [@bs.send] external foreach : (t, t => bool) => bool = "forEach";
    [@bs.send] external hasChild : (t, ~path: string) => bool = "hasChild";
    [@bs.send] external hasChildren : t => bool = "hasChildren";
    [@bs.send] external numChildren : t => int = "numChildren";
    [@bs.send] external toJson : t => Js.Json.t = "toJSON";
    [@bs.send] external val_ : t => Js.Json.t = "val";
  } = DeltaSnapshot
  and Event: {
    type t;
    [@bs.get] external data : t => DeltaSnapshot.t = "";
    [@bs.get] external params : t => Js.t('a) = "";
  } = Event;
  [@bs.val] [@bs.scope "database"] [@bs.module "firebase-functions"]
  external ref : (~path: string=?, unit) => RefBuilder.t = "";
  let onWrite = (path, cb) => RefBuilder.onWrite(cb, ref(~path, ()));
  let onCreate = (path, cb) => RefBuilder.onCreate(cb, ref(~path, ()));
  let onDelete = (path, cb) => RefBuilder.onDelete(cb, ref(~path, ()));
  let onUpdate = (path, cb) => RefBuilder.onUpdate(cb, ref(~path, ()));
  let root = event =>
    event
    |> Event.data
    |> DeltaSnapshot.ref
    |> FirebaseAdmin.Database.Reference.root;
  let adminRoot = event =>
    event
    |> Event.data
    |> DeltaSnapshot.adminRef
    |> FirebaseAdmin.Database.Reference.root;
  let child = (path, reference) =>
    FirebaseAdmin.Database.Reference.child(reference, ~path);
};
