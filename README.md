# What is it?

Bindings to Firebase Functions for ReasonML.

# What state is it in?

These bindings are partially complete. 

# How do I install it?

```sh
npm install bs-firebase-functions
# or using yarn
yarn add bs-firebase-functions
```

# How do I use it? 

```ocaml
let simpleEndpoint = 
  FirebaseFunctions.Https.(
    onRequest((req, res, _) => {
      Js.log(req |> Request.body |> Js.Json.stringifyAny);
      Js.log(req |> Request.cookies |> Js.Json.stringifyAny);
      ignore(Response.json(Js.Obj.empty(), res));
    })
  );
  
let endpointWithCookieParser =
  FirebaseFunctions.Https.(
    onRequest(
      ComposeMiddleware.make([|
        CookieParser.make(),
        (req, res, _) => {
          Js.log(req |> Request.body |> Js.Json.stringifyAny);
          Js.log(req |> Request.cookies |> Js.Json.stringifyAny);
          ignore(Response.json(Js.Obj.empty(), res));
        },
      |]),
    )
  );

let pubsubFunction =
  FirebaseFunctions.PubSub.(
    topic("incoming-topic")
    |> Topic.onPublish(event =>
         Js.Promise.resolve @@
         Js.log2(
           "processing incoming-topic",
           event |> Event.data |> Message.json,
         )
       )
  );

let databaseTrigger =
  Js.Promise.(
    FirebaseFunctions.Database.(
      onCreate(
        "/path/to/some/node",
        (event) => {
          let data = event |> Event.data |> DeltaSnapshot.val_ |> Model.decodeJson;
          resolve(data);
        }
      )
    )
  );
```

Given that this is your root JS file, it should be sufficient to expose these functions with commonjs setting for Bucklescript and Firebase Functions will find them and expose them with the same name as the functions have.
