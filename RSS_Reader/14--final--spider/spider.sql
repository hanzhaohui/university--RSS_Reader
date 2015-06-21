create table spider_rss(
Id int primary key auto_increment,
Url char(255) unique,
Title char(255) unique,
Source char(255),
Description longtext,
LinkChannel char(255)
);
