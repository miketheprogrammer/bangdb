var bangdown = require('bindings')('bangdown.node').bangdown;

console.log(bangdown);
console.log('Creating Bangodwn DB Interface');
var bd = bangdown('newdb');
console.log(bd.new);
console.log(bd.db);
console.log('Done creating interface');
console.log('executing put');
bd.put('random');

/*
console.log(bangdown.create('hello'));
console.log('create done');
bangdown.testput('helloworlddb');
console.log('put done');
*/
