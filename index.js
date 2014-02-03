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
var key, val;
for (var i = 0; i < 100; i += 1) {
  key = 'key' + i;
  bd.put(key, key);
  val = bd.get(key);
}


console.log('Test took ', new Date() - start);
console.log('Closing Database')

console.log('Iterator');
console.log(bd.iterator);
var iter = bd.iterator('key0', 'key98');
console.log(iter);
console.log(iter.next);
var v;
while(v = iter.next()) {
  console.log(v);
}
bd.close('data');
bd.free();
