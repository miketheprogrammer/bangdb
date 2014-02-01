var bangdown = require('bindings')('bangdown.node').bangdown;

console.log(bangdown);
console.log('Creating Bangodwn DB Interface');
var bd = bangdown('newdbxe');
console.log('Done creating interface');
console.log("Testing Open Put Get");
bd.open('data');

var key = (Math.floor(Math.random()) * 100).toString();
bd.put('data', key, 'value');
console.log('put');
bd.get('data', key);
console.log('done');

/*
console.log(bangdown.create('hello'));
console.log('create done');
bangdown.testput('helloworlddb');
console.log('put done');
*/

bd.put('data', 'hello', 'world');
bd.get('data', 'hello');

var start = new Date();
for (var i = 0; i < 1000; i += 1) {
  var key = 'key' + i;
  bd.put('data', key, key);
  bd.get('data', key);
}


console.log('Test took ', new Date() - start);

