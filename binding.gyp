{
  "targets": [
    {
      "target_name": "bangdb",
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
            ,"src/iterator.cc"
        ]
    }
  ]
}
