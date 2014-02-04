var bangdb = require('./index');

console.log(bangdb);
console.log('Creating Bangdb DB Interface');
var bd = bangdb('test');
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
var limit = 20000;
console.log('Iterating puts/gets');
for (var i = 0; i < limit; i += 1) {
  key = 'key' + i;
  //console.log(key);
  bd.put(key, 'val'+i);
  val = bd.get(key);
//  console.log(val);
}


console.log('Test took ', new Date() - start);

console.log('Iterator');
console.log(bd.iterator);
console.log('New Stream', 'key0', 'key'+(limit));
var iter = bd.iterator('key0', 'key'+(limit));
console.log(iter);
console.log(iter.next);

var v;
var tmp;
var count = 0;
var con = true;
console.log('Starting Iterator Benchmark');
start = new Date();
while(iter.hasNext()) {
  tmp = iter.next();
  if (tmp === v) {
    console.log("Already received", tmp);
  }
  if (tmp.length != 0)
    v = tmp;
  else con = false;
  count += 1;
  if (count === limit) {
    console.log('Limit reached',v);
  }
  if (count === limit+1) {
    console.log(tmp, tmp.length);
  }
  //console.log(v);
}
console.log('Test took ', new Date() - start, 'And Ended with Val', v, 'and count of', count);
console.log('Closing Database')
//bd.close('data');
//bd.free();

process.on('exit', function() {
//  bd.close('data');
//  bd.free();
});
