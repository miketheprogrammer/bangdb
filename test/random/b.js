db = require('./index')('data123');

db.open('test123');

db.put('hello', 'world')
console.log(db.get('hello'));

db.del('hello');
console.log('done deleting');
console.log(db.get('hello'));

