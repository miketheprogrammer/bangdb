var addon = require('bindings')('addon.node');

//var addon = require('./build/Release/addon.node');
console.log(addon);

console.log(addon.bangdown.create('hello'));
console.log('create done');
addon.bangdown.testput('helloworlddb');
console.log('put done');
