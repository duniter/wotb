{
  "targets": [
    {
      "target_name": "wotb",
      "sources": [
        "wotb.cc",
        "functions.cc",
        "wotcpp/FileWoT.cpp",
        "wotcpp/MemoryWoT.cpp",
        "wotcpp/webOfTrust.cpp",
        "wotcpp/node.cpp",
        "wotcpp/misc.cpp",
        "wotcpp/log.cpp"],
      "include_dirs" : [
        "node_modules/nan"
      ]
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}
