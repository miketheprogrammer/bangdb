var db = require('../../index')('data');

db.open('batch_data');
console.log('db open');

db.batch([
  {
      type: 'put'
    , key: 'one'
    , value: 'one'
  },
  {
      type: 'put'
    , key: 'two'
    , value: 'two'
  },
  {
      type: 'put'
    , key: 'three'
    , value: 'three'
  },
  {
      type: 'del'
    , key: 'two'
  }
], function (err, res) {
  console.log('callback called');
  console.log(err, res);
  console.log(
      db.get('one')
    , db.get('two')
    , db.get('three')
  );
});


console.log(
    db.get('one')
  , db.get('two')
  , db.get('three')
);
