<?php 
	require_once('connect.php');
	$key = $_GET['key'];
	

	$sql = "select * from spider_rss where Title like '%$key%'";
	$query = mysql_query($sql);
	if($query&&mysql_num_rows($query)){
		while($row = mysql_fetch_assoc($query)){
			$row['Title']=str_replace($key,'<font color="orange">'.$key.'</font>',$row['Title']);
			$data[] = $row;
		}
	}
	
	$sql2 = "select distinct Source from spider_rss";
	$query2 = mysql_query($sql2);
	if($query2&&mysql_num_rows($query2)){
		while($row2 = mysql_fetch_assoc($query2)){
			$data2[] = $row2;
		}
	}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<title>文章查询系统</title>
<meta name="keywords" content="" />
<meta name="description" content="" />
<link href="default.css" rel="stylesheet" type="text/css" />
</head>
<body>
<div id="wrapper">
<!-- start header -->
<div id="header">
	<div id="logo">
		<h1><a href="article.list.php">^_^&nbsp;咩咩阅读<sup></sup></a></h1>
		<h2></h2>
	</div>
	<div id="menu">
		<ul>
			<li class="active"><a href="article.list.php">文章</a></li>
			<li><a href="about.php">关于我们</a></li>
			<li><a href="contact.php">联系我们</a></li>
			<li><a href="start.spider.php">搜索最新内容</a></li>
		</ul>
	</div>
</div>
<!-- end header -->
</div>

<!-- start page -->
<div id="page">
	<!-- start content -->
	<div id="content">
	<?php
		if(empty($data)){	
			echo "<script>alert('【文章未找到】$id');window.location.href='article.list.php';</script>";
			exit;
		}else{
			foreach($data as $value){
	?>
		<div class="post">
			<h1 class="title"><a href="<?php echo $value['Url']?>"><?php echo $value['Title']?></a><span style="color:#ccc;font-size:14px;">　　来源：<!--作者放置到这里--><?php echo $value['Source']?></span></h1>
			<div class="entry">
				<!--描述search放置到这里 eg:<?php echo $value['Title']?> -->
				<font  style="font-size:80%">
				<?php 
					echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp";
					//1.得到文章的详情内容
					//$desc=html_entity_decode($value['Description'],ENT_QUOTES,"UTF-8");
					$desc=htmlspecialchars_decode($value['Description']);//有中文的时候，最好用 htmlspecialchars ，否则可能乱码
					
					//2.去掉html和php标记
					$final=strip_tags($desc);
					
					//3.打印部分内容
					//echo substr($final,0,604);
					echo mb_substr($final,0,300,'utf-8');
				?>
				</font>
			</div>
			<div class="meta">
				<p class="links"><a href="article.show.php?Id=<?php echo $value['Id']?>" class="more">查看详细</a>&nbsp;&nbsp;&raquo;&nbsp;&nbsp;</p>
			</div>
		</div>
	<?php
			}
		}
	?>
	</div>
	<!-- end content -->
	<!-- start sidebar -->
	<div id="sidebar">
		<ul>
			<!-- start subscribe -->
			<li id="search">
				<h2><b class="text">Subscribe</b></h2>
				<form method="get" action="article.add.php">
					<fieldset>
					<input type="text" id="s" name="url" value="" />
					<input type="submit" id="x" value="Add" />
					</fieldset>
				</form>
			</li>
			<!-- end subscribe -->
			<li id="search">
				<h2><b class="text">Search</b></h2>
				<form method="get" action="article.search.php">
					<fieldset>
					<input type="text" id="s" name="key" value="" />
					<input type="submit" id="x" value="Search" />
					</fieldset>
				</form>
			</li>
			<!-- start classify -->
			<li id="search">
				<h2><b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;订阅信息分类查询</b></h2>
			</li>
			
			<?php
				if(empty($data2)){	

				}else{
					foreach($data2 as $value2){
			?>
				<div class="post">

					<h2 class="title"><a href="article.classify.php?Source=<?php echo $value2['Source']?>"><?php echo $value2['Source']?></a>
						<a href="article.del.php?Source=<?php echo $value2['Source']?>"><input type="submit" id="x"  value="del" /></a>
					</h2>
				</div>
			<?php
					}
				}
			?>
			<!-- end classify -->

		</ul>
	</div>
	<!-- end sidebar -->
	<div style="clear: both;">&nbsp;</div>
</div>
<!-- end page -->
<!-- start footer -->
<div id="footer">
	<p id="legal"></p>
</div>
<!-- end footer -->
</body>
</html>
