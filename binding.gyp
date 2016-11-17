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
        '<!(node -e "require(\'nan\')")'
        ],
      'xcode_settings': {
        'MACOSX_DEPLOYMENT_TARGET': '10.7',

        'OTHER_CFLAGS': [
          "-std=c++11",
        "-stdlib=libc++"
          ],
      }, 


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
