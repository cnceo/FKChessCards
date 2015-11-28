
----------------------------------------------------------------------------------------------------

USE TZUserDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LogonByAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LogonByAccounts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

CREATE PROC GSP_GP_LogonByAccounts
	@strAccounts VARCHAR(32),
	@strPassword VARCHAR(32),
	@strClientIP VARCHAR(15)
WITH ENCRYPTION AS

SET NOCOUNT ON

-- ��������
DECLARE @UserID INT
DECLARE @Accounts VARCHAR(32)
DECLARE @Name VARCHAR(32)
DECLARE @LogonPass CHAR(32)
DECLARE @Gender BIT
DECLARE @LogonNullity BIT
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @Face VARCHAR(128)
DECLARE @MemberOrder INT
DECLARE @MemberOverDate DATETIME
DECLARE @TZPoint INT
DECLARE @LastLogonDate DATETIME
DECLARE @LastLogonIP VARCHAR(15)
DECLARE @ErrorDescribe VARCHAR(255)
DECLARE @AllLogonTimes INT

-- ִ���߼�
BEGIN

	-- Ч���ַ

	-- ��ѯ�û�
	SELECT @UserID=UserID, @Accounts=Accounts, @Name=[Name], @LogonPass=LogonPass, @Gender=Gender,@LogonNullity=LogonNullity, 
	@UserRight=UserRight, @MasterRight=MasterRight, @Face=Face, @TZPoint=TZPoint,@MemberOrder=MemberOrder, 
	@MemberOverDate=MemberOverDate, @LastLogonDate=LastLogonDate, @LastLogonIP=LastLogonIP , @AllLogonTimes=AllLogonTimes
	FROM UserAccounts WHERE Accounts=@strAccounts
	
	-- �ж��û��Ƿ����
	IF @UserID IS NULL
	BEGIN
		SET @ErrorDescribe='�ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		SELECT [ErrorDescribe]=@ErrorDescribe
		RETURN 1
	END

	-- �ж��Ƿ��ֹ��½
	IF @LogonNullity<>0
	BEGIN
		SET @ErrorDescribe='����ʻ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		SELECT [ErrorDescribe]=@ErrorDescribe
		RETURN 2
	END
	
	-- �ж��û�����
	IF @LogonPass<>@strPassword
	BEGIN
		SET @ErrorDescribe='�ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		SELECT [ErrorDescribe]=@ErrorDescribe
		RETURN 3
	END
     
	-- �жϻ�Ա�ȼ�
	IF @MemberOrder>0 AND (GetDate()>@MemberOverDate) 
	BEGIN
		SET @MemberOrder=0
		UPDATE UserAccounts SET MemberOrder=0 WHERE Accounts=@strAccounts
	END

	-- �������ݿ�
	UPDATE UserAccounts SET AllLogonTimes=AllLogonTimes+1,LastLogonDate=GETDATE(),LastLogonIP=@strClientIP
	WHERE UserID=@UserID

	-- �������
	SELECT @UserID AS UserID, @Accounts AS Accounts, @Name AS [Name], @Face AS Face, @Gender AS Gender, @UserRight AS UserRight, 
	@MasterRight AS MasterRight, @TZPoint AS TZPoint, @MemberOrder AS MemberOrder, @LastLogonDate AS LastLogonDate, @LastLogonIP AS LastLogonIP
	
END

RETURN 0

----------------------------------------------------------------------------------------------------

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

