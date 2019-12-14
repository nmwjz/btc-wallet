/*DROP DATABASE dbBTC;*/
CREATE DATABASE dbBTC;
use dbBTC;

#DROP TABLE `UserInfo`;
CREATE TABLE `UserInfo`
(
`iID`           int     auto_increment NOT NULL PRIMARY KEY COMMENT 'User ID',
`strWalletAddr` varchar(255)               NULL COMMENT 'The User Wallet',
`strPrivateKey` varchar(128)               NULL COMMENT 'The User Private',
`strWalletType` varchar(16)  DEFAULT 'BTC' NULL COMMENT 'The Wallet Type: BTC,ETH,BCH,XMR',
`strMemo`       varchar(254)               NULL COMMENT 'Memo'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


#DROP TABLE `JobInfo`;
CREATE TABLE `JobInfo`
(
`iID`           int     auto_increment NOT NULL PRIMARY KEY COMMENT 'Job ID',
`strJobBegin`   varchar(64)            NOT NULL COMMENT 'The Begin Private For Job',
`strJobCount`   varchar(64)            NOT NULL COMMENT 'The Private Count For Job',
`strStartTime`  varchar(32)                NULL COMMENT 'The Time of JobStartTime',
`strFinishTime` varchar(32)                NULL COMMENT 'The Time of JobFinishTime',
`iFlagFinish`   int                        NULL COMMENT 'The Finish Flag',
`strMemo`       varchar(254)               NULL COMMENT 'Memo'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


#DROP TABLE `JobResult`;
CREATE TABLE `JobResult`
(
`iID`           int     auto_increment NOT NULL PRIMARY KEY COMMENT 'Task ID',
`iJobID`        int                    NOT NULL COMMENT 'The JobID',
`iUserID`       int     DEFAULT 0      NOT NULL COMMENT 'The Finder ID',
`strPrivate`    varchar(64)            NOT NULL COMMENT 'The Private',
`strTime`       varchar(32)                NULL COMMENT 'The Find Time',
`strMemo`       varchar(254)               NULL COMMENT 'Memo'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


Insert into `JobInfo`(`iID`,`strJobBegin`,`strJobCount`,`strStartTime`,`iFlagFinish`,`strMemo`) 
Values( 1 , 'a000000000000000000000000000000000000000000000000000003100000000' , '80000000' , '2019-11-09 12:34:56' , 0 , 'o');
