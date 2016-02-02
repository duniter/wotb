{
  "targets": [
    {
      "target_name": "wotb",
      "sources": [
        "wotb.cc",
        "functions.cc",
        "wotcpp/webOfTrust.cpp",
        "wotcpp/node.cpp",
        "wotcpp/misc.cpp",
        "wotcpp/log.cpp"],
      "include_dirs" : [
        "node_modules/nan"
      ]
    }
  ]
}
