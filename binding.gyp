{
  "targets": [
    {
      "target_name": "addon",
      "conditions": [
         ['OS == "linux"', {
              'libraries': [
                  '-lpthread',
                  '-lbangdb'
              ]
          }]
]
      , "include_dirs"  : [
            "<!(node -e \"require('nan')\")"
        ]
              , "sources": [
            "src/addon.cc"
        ]
    }
  ]
}
