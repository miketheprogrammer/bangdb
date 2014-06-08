{
  "targets": [
    {
      "target_name": "bangdb-embedded",
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
            ,"src/scan.cc"
            ,"src/database_async.cc"
            ,"src/transaction.cc"
            ,"src/transaction_async.cc"
        ]
    },
    {
      "target_name": "bangdb-server",
      "conditions": [
         ['OS == "linux"', {
              'libraries': [
                  '-lpthread',
                  '-lbangdb-client'
              ]
          }]
]
      , "include_dirs"  : [
            "<!(node -e \"require('nan')\")"
        ]
      , "sources": [
            "src/bangdb.cc"
            ,"src/database.cc"
            ,"src/scan.cc"
            ,"src/database_async.cc"
            ,"src/transaction.cc"
            ,"src/transaction_async.cc"
        ]
    }
  ]
}
