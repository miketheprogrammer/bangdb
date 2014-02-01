var bangdown = require('bindings')('bangdown.node').bangdown;


console.log(bangdown.create('hello'));
console.log('create done');
bangdown.testput('helloworlddb');
console.log('put done');
