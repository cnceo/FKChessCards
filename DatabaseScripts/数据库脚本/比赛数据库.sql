IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'WHGameMatchDB')
	DROP database [WHGameMatchDB]
GO

CREATE database [WHGameMatchDB]  ON (NAME = N'WHGameMatchDB_dat', FILENAME = N'D:\天智游戏_棋牌游戏\天智游戏_棋牌游戏\MyGame\发\NewInfo\WHGameMatchDB.mdf' , SIZE = 2, FILEGROWTH = 10%) LOG ON (NAME = N'WHGameMatchDB_log', FILENAME = N'D:\天智游戏_棋牌游戏\天智游戏_棋牌游戏\MyGame\发\NewInfo\WHGameMatchDB.ldf' , SIZE = 2, FILEGROWTH = 10%)
 COLLATE Chinese_PRC_CI_AS
GO

exec sp_dboption N'WHGameMatchDB', N'autoclose', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'bulkcopy', N'true'
GO

exec sp_dboption N'WHGameMatchDB', N'trunc. log', N'true'
GO

exec sp_dboption N'WHGameMatchDB', N'torn page detection', N'true'
GO

exec sp_dboption N'WHGameMatchDB', N'read only', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'dbo use', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'single', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'autoshrink', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'ANSI null default', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'recursive triggers', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'ANSI nulls', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'concat null yields null', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'cursor close on commit', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'default to local cursor', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'quoted identifier', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'ANSI warnings', N'false'
GO

exec sp_dboption N'WHGameMatchDB', N'auto create statistics', N'true'
GO

exec sp_dboption N'WHGameMatchDB', N'auto update statistics', N'true'
GO

if( (@@microsoftversion / power(2, 24) = 8) and (@@microsoftversion & 0xffff >= 724) )
	exec sp_dboption N'WHGameMatchDB', N'db chaining', N'false'
GO

use [WHGameMatchDB]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[GameLocker]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[GameLocker]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[GameLogonLog]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[GameLogonLog]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[GameScore]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[GameScore]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[GameScoreLog]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[GameScoreLog]
GO

CREATE TABLE [dbo].[GameLocker] (
	[UserID] [int] NOT NULL ,
	[KindID] [int] NOT NULL ,
	[ServerID] [int] NOT NULL ,
	[LockDate] [smalldatetime] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[GameLogonLog] (
	[ID] [int] IDENTITY (1, 1) NOT NULL ,
	[UserID] [int] NOT NULL ,
	[Score] [int] NOT NULL ,
	[WinCount] [int] NOT NULL ,
	[LostCount] [int] NOT NULL ,
	[DrawCount] [int] NOT NULL ,
	[FleeCount] [int] NOT NULL ,
	[KindID] [int] NOT NULL ,
	[ServerID] [int] NOT NULL ,
	[ClientIP] [varchar] (15) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[LogonTime] [smalldatetime] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[GameScore] (
	[UserID] [int] NOT NULL ,
	[Score] [int] NOT NULL ,
	[WinCount] [int] NOT NULL ,
	[LostCount] [int] NOT NULL ,
	[DrawCount] [int] NOT NULL ,
	[FleeCount] [int] NOT NULL ,
	[UserRight] [int] NOT NULL ,
	[MasterRight] [int] NOT NULL ,
	[PlayTimeCount] [int] NOT NULL ,
	[AllLogonTimes] [int] NOT NULL ,
	[RegisterDate] [datetime] NOT NULL ,
	[LastLogonDate] [datetime] NOT NULL ,
	[RegisterIP] [varchar] (15) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[LastLogonIP] [varchar] (15) COLLATE Chinese_PRC_CI_AS NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[GameScoreLog] (
	[ID] [int] IDENTITY (1, 1) NOT NULL ,
	[UserID] [int] NOT NULL ,
	[LeftTime] [smalldatetime] NOT NULL ,
	[Score] [int] NOT NULL ,
	[WinCount] [int] NOT NULL ,
	[LostCount] [int] NOT NULL ,
	[DrawCount] [int] NOT NULL ,
	[FleeCount] [int] NOT NULL ,
	[Experience] [int] NOT NULL ,
	[PlayTimeCount] [int] NOT NULL ,
	[OnLineTimeCount] [int] NOT NULL ,
	[KindID] [int] NOT NULL ,
	[ServerID] [int] NOT NULL ,
	[ClientIP] [varchar] (15) COLLATE Chinese_PRC_CI_AS NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[GameLocker] WITH NOCHECK ADD 
	CONSTRAINT [PK_LogonInfo] PRIMARY KEY  CLUSTERED 
	(
		[UserID]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[GameLogonLog] WITH NOCHECK ADD 
	CONSTRAINT [PK_GameLogonLog] PRIMARY KEY  CLUSTERED 
	(
		[ID]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[GameScore] WITH NOCHECK ADD 
	CONSTRAINT [PK_GameCurrency] PRIMARY KEY  CLUSTERED 
	(
		[UserID]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[GameScoreLog] WITH NOCHECK ADD 
	CONSTRAINT [PK_GameScoreLog] PRIMARY KEY  CLUSTERED 
	(
		[ID]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[GameLocker] ADD 
	CONSTRAINT [DF_GameLocker_LockDate] DEFAULT (getdate()) FOR [LockDate]
GO

ALTER TABLE [dbo].[GameLogonLog] ADD 
	CONSTRAINT [DF_GameLogonLog_LogonTime] DEFAULT (getdate()) FOR [LogonTime]
GO

 CREATE  INDEX [IX_GameLogonLog_UserID] ON [dbo].[GameLogonLog]([UserID]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

ALTER TABLE [dbo].[GameScore] ADD 
	CONSTRAINT [DF_GameScore_Score] DEFAULT (100) FOR [Score],
	CONSTRAINT [DF_GameScore_WinCount] DEFAULT (0) FOR [WinCount],
	CONSTRAINT [DF_GameScore_LostCount] DEFAULT (0) FOR [LostCount],
	CONSTRAINT [DF_GameScore_DrawCount] DEFAULT (0) FOR [DrawCount],
	CONSTRAINT [DF_GameScore_FreeCount] DEFAULT (0) FOR [FleeCount],
	CONSTRAINT [DF_GameScore_UserRight] DEFAULT (0) FOR [UserRight],
	CONSTRAINT [DF_GameScore_MasterRight] DEFAULT (0) FOR [MasterRight],
	CONSTRAINT [DF_GameScore_PlayTimeCount] DEFAULT (0) FOR [PlayTimeCount],
	CONSTRAINT [DF_GameScore_AllLogonTimes] DEFAULT (0) FOR [AllLogonTimes],
	CONSTRAINT [DF_GameScore_RegisterDate] DEFAULT (getdate()) FOR [RegisterDate],
	CONSTRAINT [DF_GameScore_LastLogonDate] DEFAULT (getdate()) FOR [LastLogonDate]
GO

ALTER TABLE [dbo].[GameScoreLog] ADD 
	CONSTRAINT [DF_GameScore_LeftTime] DEFAULT (getdate()) FOR [LeftTime]
GO


exec sp_addextendedproperty N'MS_Description', N'类型标识', N'user', N'dbo', N'table', N'GameLocker', N'column', N'KindID'
GO
exec sp_addextendedproperty N'MS_Description', N'锁定时间', N'user', N'dbo', N'table', N'GameLocker', N'column', N'LockDate'
GO
exec sp_addextendedproperty N'MS_Description', N'房间标识', N'user', N'dbo', N'table', N'GameLocker', N'column', N'ServerID'
GO
exec sp_addextendedproperty N'MS_Description', N'用户标识', N'user', N'dbo', N'table', N'GameLocker', N'column', N'UserID'


GO


exec sp_addextendedproperty N'MS_Description', N'登陆地址', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'ClientIP'
GO
exec sp_addextendedproperty N'MS_Description', N'和局', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'DrawCount'
GO
exec sp_addextendedproperty N'MS_Description', N'逃局', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'FleeCount'
GO
exec sp_addextendedproperty N'MS_Description', N'顺序号码', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'ID'
GO
exec sp_addextendedproperty N'MS_Description', N'类型标识', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'KindID'
GO
exec sp_addextendedproperty N'MS_Description', N'登陆时间', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'LogonTime'
GO
exec sp_addextendedproperty N'MS_Description', N'输局', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'LostCount'
GO
exec sp_addextendedproperty N'MS_Description', N'用户积分', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'Score'
GO
exec sp_addextendedproperty N'MS_Description', N'房间标识', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'ServerID'
GO
exec sp_addextendedproperty N'MS_Description', N'用户 ID', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'UserID'
GO
exec sp_addextendedproperty N'MS_Description', N'胜局', N'user', N'dbo', N'table', N'GameLogonLog', N'column', N'WinCount'


GO


exec sp_addextendedproperty N'MS_Description', N'登陆次数', N'user', N'dbo', N'table', N'GameScore', N'column', N'AllLogonTimes'
GO
exec sp_addextendedproperty N'MS_Description', N'和局', N'user', N'dbo', N'table', N'GameScore', N'column', N'DrawCount'
GO
exec sp_addextendedproperty N'MS_Description', N'逃局', N'user', N'dbo', N'table', N'GameScore', N'column', N'FleeCount'
GO
exec sp_addextendedproperty N'MS_Description', N'登陆时间', N'user', N'dbo', N'table', N'GameScore', N'column', N'LastLogonDate'
GO
exec sp_addextendedproperty N'MS_Description', N'登陆地址', N'user', N'dbo', N'table', N'GameScore', N'column', N'LastLogonIP'
GO
exec sp_addextendedproperty N'MS_Description', N'输局', N'user', N'dbo', N'table', N'GameScore', N'column', N'LostCount'
GO
exec sp_addextendedproperty N'MS_Description', N'管理权限', N'user', N'dbo', N'table', N'GameScore', N'column', N'MasterRight'
GO
exec sp_addextendedproperty N'MS_Description', N'游戏时间', N'user', N'dbo', N'table', N'GameScore', N'column', N'PlayTimeCount'
GO
exec sp_addextendedproperty N'MS_Description', N'注册时间', N'user', N'dbo', N'table', N'GameScore', N'column', N'RegisterDate'
GO
exec sp_addextendedproperty N'MS_Description', N'注册地址', N'user', N'dbo', N'table', N'GameScore', N'column', N'RegisterIP'
GO
exec sp_addextendedproperty N'MS_Description', N'积分', N'user', N'dbo', N'table', N'GameScore', N'column', N'Score'
GO
exec sp_addextendedproperty N'MS_Description', N'用户标识', N'user', N'dbo', N'table', N'GameScore', N'column', N'UserID'
GO
exec sp_addextendedproperty N'MS_Description', N'用户权限', N'user', N'dbo', N'table', N'GameScore', N'column', N'UserRight'
GO
exec sp_addextendedproperty N'MS_Description', N'胜局', N'user', N'dbo', N'table', N'GameScore', N'column', N'WinCount'


GO


exec sp_addextendedproperty N'MS_Description', N'登陆地址', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'ClientIP'
GO
exec sp_addextendedproperty N'MS_Description', N'和局', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'DrawCount'
GO
exec sp_addextendedproperty N'MS_Description', N'经验数值', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'Experience'
GO
exec sp_addextendedproperty N'MS_Description', N'逃局', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'FleeCount'
GO
exec sp_addextendedproperty N'MS_Description', N'顺序索引', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'ID'
GO
exec sp_addextendedproperty N'MS_Description', N'类型标识', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'KindID'
GO
exec sp_addextendedproperty N'MS_Description', N'登陆时间', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'LeftTime'
GO
exec sp_addextendedproperty N'MS_Description', N'输局', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'LostCount'
GO
exec sp_addextendedproperty N'MS_Description', N'在线时间', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'OnLineTimeCount'
GO
exec sp_addextendedproperty N'MS_Description', N'游戏时间', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'PlayTimeCount'
GO
exec sp_addextendedproperty N'MS_Description', N'积分', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'Score'
GO
exec sp_addextendedproperty N'MS_Description', N'房间标识', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'ServerID'
GO
exec sp_addextendedproperty N'MS_Description', N'用户 ID', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'UserID'
GO
exec sp_addextendedproperty N'MS_Description', N'胜局', N'user', N'dbo', N'table', N'GameScoreLog', N'column', N'WinCount'


GO

