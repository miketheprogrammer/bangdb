{
  "targets": [
    {
      "target_name": "bangdown",
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
            "src/bangdown.cc"
            ,"src/database.cc"
        ]
    }
  ]
}
