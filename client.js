const addon = require('./build/Release/addon');

buffer = Buffer.alloc(4)
map = new addon.FileMapping();
lock = new addon.Mutex();

map.createMapping(null, 'howard_mem_map', 4);
lock.create('howard_mem_map_lock');

while(1)
{
  lock.waitMultiple([lock], true, addon.INFINITE);
  map.readInto(0, 4, buffer);
  lock.release();

  var i = buffer.readInt32LE(0);
  console.log ('Read: ' + i);

  if (i == 59) break;
}

map.closeMapping();
lock.close();
