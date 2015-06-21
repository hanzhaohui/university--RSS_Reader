<?php
	
	header("Content-type: text/html; charset=utf-8");
	
	//echo "<script>alert('搜索完毕');</script>";

	//考虑安全性和超时
	
	//1.直接显示在浏览器
	//system('./spidercpp.o');//调用c++程序(会将执行结果输出)
	//system("./spidercpp.o > /tmp/null &");//重定向到文件或流，并放入后台执行
	//system("./spidercpp.o > spider.result.txt &");
	system("./spidercpp.o");
	
	//2.将C++程序的执行结果保存在数组变量
	/*
	exec('./spidercpp.o',$return_array,$return_flag);//return_var –0则成功，1则失败
	if($return_flag==1)
	{
		echo "<script>alert('搜索失败');window.location.href='article.list.php';</script>";
	}
	//var_dump($return_array);  
	//print_r($return_array);
	foreach($return_array as $return_val)
		echo $return_val;
	*/
	
	
	//3.标准输出
	//passthru ('./spidercpp.o',$return_val);//执行外部程序并且显示原始输出
	
	
	echo "<script>alert('搜索程序已搜索完毕');window.location.href='article.list.php';</script>";
?>