var bangdown = require('bindings')('bangdown.node').bangdown;

console.log(bangdown);
console.log('Creating Bangodwn DB Interface');
var bd = bangdown('newdbxe');
console.log('Done creating interface');
console.log("Testing Open Put Get");

bd.open('data');

var key = (Math.floor(Math.random()) * 100).toString();
bd.put(key, 'value');
console.log('put');
bd.get(key);
console.log('done');

/*
console.log(bangdown.create('hello'));
console.log('create done');
bangdown.testput('helloworlddb');
console.log('put done');
*/


var start = new Date();
for (var i = 0; i < 1000000; i += 1) {
  var key = 'key' + i;
  bd.put(key, key);
  bd.get(key);
}


console.log('Test took ', new Date() - start);

