<?php
	
	header("Content-type: text/html; charset=utf-8");
	$id=$_GET['url'];
	
	//首先对url做验证
	if(!filter_var($id, FILTER_VALIDATE_URL))
	{
		echo "<script>alert('[订阅feed失败，Url地址不可用]$id');window.location.href='article.list.php';</script>";
		exit;
	}

	/*-----------read by @ackhan-----------*/
	//1.读打开配置文件，判断是否为重复订阅
	//$fp=fopen("/home/wgluohappy/7--spider/spiderbot/reader.conf","r");
	$fp=fopen("reader.conf","r");
	if(!$fp)
	{
		echo "article is't exist in read";
	}
	//2.读取文件内容
	while (!feof($fp))
	{
		$data=fgets($fp);
		if($data[0]=='#') continue;
		$bn=is_int(strpos($data,$id));
		if($bn)
		{
			echo "<script>alert('重复订阅:$id');window.location.href='article.list.php';</script>";
			exit;
		}
		else
		{
			echo "$data<br>";
		}
	}
	//3.关闭文件
	fclose($fp);
	
	
	
	
	/*-----------write by @ackhan-----------*/
	//1.写打开配置文件，增加订阅
	$james=fopen("reader.conf","a");
	if(!$james)
	{
		echo "<script>alert('订阅失败:$id');window.location.href='article.list.php';</script>";
		exit;
	}
	//2.写入文件内容
	$id_final=sprintf("%s%s",$id,"\n");
	fwrite($james,$id_final);
	//3.关闭文件
	fclose($james);

	
	echo "<script>alert('订阅feed成功:$id');window.location.href='article.list.php';</script>";
?>
