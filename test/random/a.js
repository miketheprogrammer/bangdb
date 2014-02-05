db = require('./index')('data123');

db.open('test123')
console.log(db.get('hello'));
db.put('hello', 'world', function(err) {
if(err) console.log(err);
db.get('hello', function(err, res) {
  console.log('callback called');
  console.log(err, res);
});
});

console.log(db.get('hello'));
