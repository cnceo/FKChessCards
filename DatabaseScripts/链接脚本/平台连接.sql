
-- 变量定义
DECLARE @DBName VARCHAR(32)
DECLARE @DBUser VARCHAR(32)
DECLARE @DBPassword VARCHAR(32)
DECLARE @DBSourceAddr VARCHAR(32)
DECLARE @LinkServerName VARCHAR(32)

-- 设置变量
SET @LinkServerName='TZServerInfoDBServer'
SET @DBSourceAddr='192.168.1.103'
SET @DBName='TZServerInfoDB'
SET @DBUser='sa'
SET @DBPassword='123456'

-- 删除连接服务器
IF EXISTS(SELECT SrvName FROM MASTER.DBO.SYSSERVERS WHERE SrvName=@LinkServerName)
BEGIN
  EXEC SP_DROPSERVER @LinkServerName,'DropLogins'
END

-- 建立链接服务器
EXEC SP_ADDLINKEDSERVER 
	@Server=@LinkServerName, 
	@SrvProduct=@LinkServerName,
	@Provider='SQLOLEDB',
	@DataSrc=@DBSourceAddr,
	@Location='',
	@ProvStr='',
	@Catalog=@DBName

-- 用户映射
EXEC SP_ADDLINKEDSRVLOGIN @RmtSrvName=@LinkServerName,@Useself='false',@LocalLogin=NULL,@RmtUser=@DBUser,@RmtPassword=@DBPassword

-- 设置链接服务器
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='RPC',@OptValue='true'
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='RPC OUT',@OptValue='true'
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='Data Access',@OptValue='true'
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='Collation CompaTible',@OptValue='true'
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='Use Remote Collation',@OptValue='true'

-- 链接服务器状态
EXEC SP_HELPSERVER @LinkServerName