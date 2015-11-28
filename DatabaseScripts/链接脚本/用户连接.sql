
-- ��������
DECLARE @DBName VARCHAR(32)
DECLARE @DBUser VARCHAR(32)
DECLARE @DBPassword VARCHAR(32)
DECLARE @DBSourceAddr VARCHAR(32)
DECLARE @LinkServerName VARCHAR(32)

-- ���ñ���
SET @LinkServerName='TZUserDBServer'
SET @DBSourceAddr='192.168.1.103'
SET @DBName='TZUserDB'
SET @DBUser='sa'
SET @DBPassword='123456'

-- ɾ�����ӷ�����
IF EXISTS(SELECT SrvName FROM MASTER.DBO.SYSSERVERS WHERE SrvName=@LinkServerName)
BEGIN
  EXEC SP_DROPSERVER @LinkServerName,'DropLogins'
END

-- �������ӷ�����
EXEC SP_ADDLINKEDSERVER 
	@Server=@LinkServerName, 
	@SrvProduct=@LinkServerName,
	@Provider='SQLOLEDB',
	@DataSrc=@DBSourceAddr,
	@Location='',
	@ProvStr='',
	@Catalog=@DBName

-- �û�ӳ��
EXEC SP_ADDLINKEDSRVLOGIN @RmtSrvName=@LinkServerName,@Useself='false',@LocalLogin=NULL,@RmtUser=@DBUser,@RmtPassword=@DBPassword

-- �������ӷ�����
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='RPC',@OptValue='true'
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='RPC OUT',@OptValue='true'
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='Data Access',@OptValue='true'
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='Collation CompaTible',@OptValue='true'
EXEC SP_SERVEROPTION @Server=@LinkServerName,@OptName='Use Remote Collation',@OptValue='true'

-- ���ӷ�����״̬
EXEC SP_HELPSERVER @LinkServerName