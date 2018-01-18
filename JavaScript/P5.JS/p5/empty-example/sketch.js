var z = {a:0,
         b:0,
};

function setup(){
  createCanvas(100, 100);
  background(51);
  for(var i=0; i<height; i++){
    for(var j=0; j<width; j++){
      z.a = 0;
      z.b = 0;
      var x = map(i, 0, height, -2, 2);
      var y = map(j, 0, width, -2, 2);
      var n=0;
      while(n<100){
        z.a = z.a*z.a-z.b*z.b + x;
        z.b = 2 * z.a * z.b + y;
        if(abs(z.a+z.b)>16.0)
           break;
      }
      var bright = map(n, 0, 100, 0, 255);
      fill(bright);
      point(i, j);
    }
  }
}

function draw(){


}


