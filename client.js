const addon = require('./build/Release/addon');

buffer = Buffer.alloc(4)
map = new addon.FileMapping();
map.createMapping(null, 'howard_mem_map', 4);

while(1)
{
  map.readInto(0, 4, buffer);
  var i = buffer.readInt32LE(0);
  console.log ('Read: ' + i);
}

map.closeMapping();
