```
..\third_party\protobuf\bin\protoc.exe  -I="." --grpc_out="." --plugin=protoc-gen-grpc="..\third_party\grpc\bin\grpc_cpp_plugin.exe" "message.proto"
```

```
`..\third_party\protobuf\bin\protoc.exe --cpp_out=. "message.proto"`
```

