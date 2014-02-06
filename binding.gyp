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
            "src/bangdb.cc"
            ,"src/database.cc"
            ,"src/iterator.cc"
            ,"src/database_async.cc"
            ,"src/batch.cc"
            ,"src/batch_async.cc"
        ]
    }
  ]
}
