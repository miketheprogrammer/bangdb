db = require('../../index')('data');
db.open('testbatch');

batch = db.batch();

batch = batch.put('one', 'two')
.put('two', 'two')
.put('three', 'three')
.delete('two','two')

console.log(
    db.get('one')
  , db.get('two')
  , db.get('three')
);

batch.write();

console.log(
    db.get('one')
  , db.get('two')
  , db.get('three')
);
