
----------------------------------------------------------------------------------------------------

USE WHGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LogonByUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LogonByUserID]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

CREATE PROC GSP_GR_LogonByUserID
	@dwUserID INT,
	@strPassword CHAR(32),
	@wKindID INT,
	@wServerID INT,
	@strClientIP VARCHAR(15)

	-- ���ܽű�
	WITH ENCRYPTION 

AS

SET NOCOUNT ON

-- �û�����
DECLARE @UserID INT
DECLARE @Accounts VARCHAR(32)
DECLARE @LogonPass VARCHAR(32)
DECLARE @Gender BIT
DECLARE @LogonNullity BIT
DECLARE @UserRight INT
DECLARE @MasterRight INT
DECLARE @FaceID INT
DECLARE @GroupID INT
DECLARE @MemberOrder INT
DECLARE @MemberOverDate datetime
DECLARE @Experience INT
DECLARE @ErrorDescribe VARCHAR(128)

-- ���ű���
DECLARE @GroupName VARCHAR(32)

-- ��Ϸ����
DECLARE @Score INT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @GameUserRight INT
DECLARE @GameMasterRight INT

-- ִ���߼�
BEGIN

	-- Ч���ַ

	-- ��ѯ�û�
	SELECT @UserID=UserID, @Accounts=Accounts, @LogonPass=LogonPass, @Gender=Gender, @LogonNullity=LogonNullity, 
		@UserRight=UserRight, @MasterRight=MasterRight, @FaceID=FaceID, @GroupID=GroupID,@MemberOrder=MemberOrder, 
		@MemberOverDate=MemberOverDate, @Experience=Experience 
	FROM WHGameUserDBServer.WHGameUserDB.dbo.UserAccounts WHERE UserID=@dwUserID
	
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

	-- ��������
	IF EXISTS (SELECT UserID FROM GameLocker WHERE UserID=@dwUserID)
	BEGIN
		SET @ErrorDescribe='���Ѿ��ڱ�����Ϸ�����ˣ�����ͬʱ�ڽ������Ϸ�����ˣ�'
		SELECT [ErrorDescribe]=@ErrorDescribe
		RETURN 2
	END
	INSERT GameLocker (UserID,KindID,ServerID) VALUES (@dwUserID,@wKindID,@wServerID)

	-- ��ȡ��Ϸ��Ϣ
	SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, 
		   @GameUserRight=UserRight, @GameMasterRight=MasterRight FROM GameScore WHERE UserID=@dwUserID
	IF @Score IS NULL
	BEGIN
		-- ������Ϸ��Ϣ
		INSERT INTO GameScore (UserID, LastLogonIP, RegisterIP)	VALUES (@dwUserID,@strClientIP,@strClientIP)

		-- ��ȡ��Ϸ��Ϣ
		SELECT @Score=Score, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount, @DrawCount=DrawCount, @FleeCount=FleeCount, 
			   @GameUserRight=UserRight, @GameMasterRight=MasterRight FROM GameScore WHERE UserID=@dwUserID
	END

	-- ��ȡ������Ϣ
	SET @GroupName=''

	-- �������ݿ�
	UPDATE GameScore SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=Getdate(), LastLogonIP=@strClientIP
	WHERE UserID=@dwUserID

	-- д���¼
	INSERT GameLogonLog (UserID, Score, WinCount, LostCount, DrawCount, FleeCount, ClientIP, KindID, ServerID) 
	VALUES (@UserID, @Score, @WinCount, @LostCount, @DrawCount, @FleeCount, @strClientIP, @wKindID, @wServerID)
	
	-- ��������
	SET @GameUserRight=268435456

	-- �������
	SELECT @UserID AS UserID, @Accounts AS Accounts, @FaceID AS FaceID, @Gender AS Gender, 
		@UserRight|@GameUserRight AS UserRight, @MasterRight|@GameMasterRight AS MasterRight, @GroupID AS GroupID, 
		@GroupName AS GroupName, @MemberOrder AS MemberOrder, @Experience AS Experience, @Score AS Score,
		@WinCount AS WinCount, @LostCount AS LostCount, @DrawCount AS DrawCount, @FleeCount AS FleeCount 
	
END

RETURN 0

----------------------------------------------------------------------------------------------------

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

