var clientWidth = document.documentElement.clientWidth;  //变量暂未使用，预留

/*定义一个二维数组，作为15X15的棋盘*/
var cbArray = new Array(15);
for(var i = 0; i < cbArray.length; i++){
    cbArray[i] = new Array(cbArray.length);
    for(var j = 0;j < cbArray.length; j++){
        cbArray[i][j] = 0;
    }
}

/*初始化棋子*/
var black = new Image();
var white = new Image();
black.src = "images/black.png";
white.src = "images/white.png";

/*棋盘初始化*/
var canvasOfMain = document.getElementById('main');
var ctxOfMain = canvasOfMain.getContext("2d");  //获取该canvas的2D绘图环境对象
ctxOfMain.strokeStyle = "#333";

//下面这四行绘制正方形，对于15X15的棋盘，横竖绘制14个正方形即可
for(var m = 0;m < cbArray.length - 1; m++){
    for(var n = 0; n < cbArray.length - 1; n++){
        ctxOfMain.strokeRect(m*40+40, n*40+40, 40, 40);  //绘制40的小正方形
    }
}

/*棋盘状态标志位*/
var isBlack = true; //当前棋子是否为黑色
var isGameOver = false; //当前局是否结束，预留变量

/*底部状态栏初始化*/
var canvasOfStatusBar = document.getElementsByClassName('statusbar');
var ctxOfStatusBar = canvasOfStatusBar[0].getContext("2d");

ctxOfStatusBar.beginPath();
ctxOfStatusBar.font=("100px Georgia");
ctxOfStatusBar.fillStyle="#F70707";
//ctxOfStatusBar.fillText("Hello",40,100);

/*点击棋盘开始下棋*/
canvasOfMain.onclick = function play(e){
    console.log("鼠标点击的坐标(" + e.clientX + "," + e.clientY + ")");

    //获取棋盘相对外边框左边和顶边的偏移量，即确定棋盘左上角那个点的坐标
    var l = this.offsetLeft + 40;
    var t = this.offsetTop + 40;

    //获取点击的位置相对棋盘左上角那个点的坐标的偏移量
    var x = e.clientX - l;
    var y = e.clientY - t;

    var row = 0, col = 0;

    //确定棋子二维数组坐标，左上角为(0,0)，右下角为(14,14)
    if( x > 0 ){
        if( x % 40 < 20 ){
            col = parseInt(x/40);
        }else{
            col = parseInt(x/40)+1;
        }
    }

    if( y > 0 ){
        if( y % 40 < 20 ){
            row = parseInt(y/40);
        }else{
            row = parseInt(y/40)+1;
        }
    }

    console.log("棋盘坐标(" + col + "," + row + ")");

    if( cbArray[row][col] === 0 ){
        if(isBlack){//下黑子
            ctxOfMain.drawImage(black, col*40+22, row*40+22);//理论上是加20，这里加22属于微调，让棋子摆在正中央
            isBlack = false;
            cbArray[row][col] = 1; //黑子为1
            check(1, row, col);
        }else{
            ctxOfMain.drawImage(white, col*40+22, row*40+22);
            isBlack = true;
            cbArray[row][col] = 2; //白子为2
            check(2, row, col);
        }
    }
};

/*判断棋局是否结束*/
function check(color, row, col){
    var rowBak = row, colBak = col, total = 1;

    //判断东西方向（←→）是否有五个
    while( col > 0 && cbArray[row][col-1] === color ){  //当前棋子左边还有几个同色的棋子
        total++;
        col--;
    }
    row = rowBak;
    col = colBak;
    while( col+1<15 && cbArray[row][col+1] === color ){  //当前棋子右边还有几个同色的棋子
        col++;
        total++;
    }
    isWin();

    //判断南北方向（↑↓）是否有五个
    reset();
    while( row > 0 && cbArray[row-1][col] === color ){   //当前棋子上边还有几个同色的棋子
        total++;
        row--;
    }
    row = rowBak;
    col = colBak;
    while( row+1 < 15 && cbArray[row+1][col] === color ){  //当前棋子下边还有几个同色的棋子
        total++;
        row++;
    }
    isWin();

    //判断东南和西北方向（↖↘）是否有五个
    reset();
    while( row > 0 && col>0 && cbArray[row-1][col-1] === color ){ //当前棋子左上边还有几个同色的棋子
        row--;
        col--;
        total++;
    }
    row = rowBak;
    col = colBak;
    while( row+1 < 15 && col+1 < 15 && cbArray[row+1][col+1] === color ){  //当前棋子右下边还有几个同色的棋子
        row++;
        col++;
        total++;
    }
    isWin();

    //判断东北和西南方向（↙↗）有五个
    reset();
    while( row > 0 && col+1 < 15 && cbArray[row-1][col+1] === color ){  //当前棋子右上边还有几个同色的棋子
        row--;
        col++;
        total++;
    }
    row = rowBak;
    col = colBak;
    while( row+1 < 15 && col > 0 && cbArray[row+1][col-1] === color ){   //当前棋子左下边还有几个同色的棋子
        row++;
        col--;
        total++;
    }
    isWin();

    //判断是否结束
    function isWin(){
        if( total >= 5 ){
            if( color === 1 ){
                // alert("黑子赢");
                // canvasOfStatusBar[0].innerHTML="黑子赢";
                ctxOfStatusBar.clearRect(0, 0, canvasOfStatusBar[0].width, canvasOfStatusBar[0].height);
                ctxOfStatusBar.fillText("黑子赢", 0, 100);
            }else{
                // alert("白子赢");
                // canvasOfStatusBar[0].innerHTML="白子赢";
                ctxOfStatusBar.clearRect(0, 0, canvasOfStatusBar[0].width, canvasOfStatusBar[0].height);
                ctxOfStatusBar.fillText("白子赢", 0, 100);
            }
        }
    }

    //重置当前棋子的坐标和连子数
    function reset(){
        rowBak = row;
        colBak = col;
        total = 1;
    }
}