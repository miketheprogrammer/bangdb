db1 = require('../../index')('data');
db2 = require('../../index')('data');

db1.open('table1');
db2.open('table2');


db1.put('one', 'one');
db2.put('two', 'two');


console.log(db1.get('one'));
console.log(db2.get('two'));

console.log(db1.get('two'));
console.log(db2.get('one'));
