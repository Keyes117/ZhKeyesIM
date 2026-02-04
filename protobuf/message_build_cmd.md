```
..\third_party\grpc\bin\protoc.exe  -I="." --grpc_out="." --plugin=protoc-gen-grpc="..\third_party\grpc\bin\grpc_cpp_plugin.exe" "message.proto"
```

```
..\third_party\grpc\bin\protoc.exe --cpp_out=. "message.proto"
```

1. `.\b2.exe install --toolset=msvc-17.2 --build-type=complete --prefix="D:\Resource\boost_1_81_0" link=static runtime-link=shared threading=multi debug release`
