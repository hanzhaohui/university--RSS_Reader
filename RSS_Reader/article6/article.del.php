<?php
	require_once('connect.php');
	$id=$_GET['Source'];
	
	$sql="SELECT DISTINCT LinkChannel FROM spider_rss WHERE Source ='$id'";
	$query=mysql_query($sql);
	if($query&&mysql_num_rows($query)){
		$row = mysql_fetch_assoc($query);
	}else{
		//echo "该来源已不存在！！！";
		echo "<script>alert('该来源已不存在！！！');window.location.href='article.list.php';</script>";
		exit;
	}
	
	
	//1.delete in file
	$link_channel=$row['LinkChannel'];
	$link_final=sprintf("%s%s",'#',$link_channel);
	
	$origin=file_get_contents("reader.conf");
	$update_str=str_replace("$link_channel","$link_final",$origin);//error:'$link_channel'
	$writenum=file_put_contents("reader.conf",$update_str);//返回写入字节数

  
	//2.delete in mysql
	$sql2="delete from spider_rss where Source='$id'";
	$flag=mysql_query($sql2);//执行成功返回TRUE

	//3.验证执行结果并输出
	if($writenum>=0 && $flag==TRUE )
	{
		echo "<script>alert('删除成功！！！');window.location.href='article.list.php';</script>";
	}
	else
	{
		echo "<script>alert('删除失败！！！');window.location.href='article.list.php';</script>";
	}
?>
