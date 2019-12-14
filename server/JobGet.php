<?php
function num_to_char( $iNum )
{
	if( 0 == $iNum )
		return '0';
	if( 1 == $iNum )
		return '1';
	if( 2 == $iNum )
		return '2';
	if( 3 == $iNum )
		return '3';
	if( 4 == $iNum )
		return '4';
	if( 5 == $iNum )
		return '5';
	if( 6 == $iNum )
		return '6';
	if( 7 == $iNum )
		return '7';
	if( 8 == $iNum )
		return '8';
	if( 9 == $iNum )
		return '9';
	if( 10 == $iNum )
		return 'a';
	if( 11 == $iNum )
		return 'b';
	if( 12 == $iNum )
		return 'c';
	if( 13 == $iNum )
		return 'd';
	if( 14 == $iNum )
		return 'e';
	if( 15 == $iNum )
		return 'f';

	return '';
}

function char_to_num( $SrcChar )
{
	if( '0' == $SrcChar )
		return 0;
	if( '1' == $SrcChar )
		return 1;
	if( '2' == $SrcChar )
		return 2;
	if( '3' == $SrcChar )
		return 3;
	if( '4' == $SrcChar )
		return 4;
	if( '5' == $SrcChar )
		return 5;
	if( '6' == $SrcChar )
		return 6;
	if( '7' == $SrcChar )
		return 7;
	if( '8' == $SrcChar )
		return 8;
	if( '9' == $SrcChar )
		return 9;
	if( ( 'a' == $SrcChar ) || ( 'A' == $SrcChar ) )
		return 10;
	if( ( 'b' == $SrcChar ) || ( 'B' == $SrcChar ) )
		return 11;
	if( ( 'c' == $SrcChar ) || ( 'C' == $SrcChar ) )
		return 12;
	if( ( 'd' == $SrcChar ) || ( 'D' == $SrcChar ) )
		return 13;
	if( ( 'e' == $SrcChar ) || ( 'E' == $SrcChar ) )
		return 14;
	if( ( 'f' == $SrcChar ) || ( 'F' == $SrcChar ) )
		return 15;

	return 0;
}

//??????
function Cal_Start_Key( $strPrivKey , $strCount )
{
	$strDest = "";
	
	$iSize1 = strlen($strPrivKey);
	$iSize2 = strlen($strCount);

	//??
	$iCarry = 0;
	for($i=0; $i<$iSize1; $i++)
	{
		//?$strPrivKey??????????
		$iNum1=0;
		if( ($iSize1 - $i)>=0 )
			$iNum1 = char_to_num( $strPrivKey[$iSize1 - $i-1] );
		else
			$iNum1 = 0;

		//?$strCount??????????
		$iNum2=0;
		if( ($iSize2 - $i)>=0 )
			$iNum2 = char_to_num( $strCount[$iSize2 - $i-1] );
		else
			$iNum2 = 0;

		//??????
		$iNum = $iNum1 + $iNum2 + $iCarry;
		if( $iNum >= 16 )
		{
			$iCarry = 1;
			$iNum = $iNum - 16;
		}
		else
			$iCarry = 0;

		//?????????
		$strDest = num_to_char( $iNum ) . $strDest;
	}

	return $strDest;
}

//??????JOB,??Job
function Create_New_Job($connect_db)
{
	//?????JOB,???????Job?????
	$strSQL = "SELECT iID,strJobBegin,strJobCount FROM JobInfo Order by iID DESC limit 0,1";
	$oRecordset = mysqli_query($connect_db,$strSQL);
	if (!$oRecordset)
		return NULL;
	//?????JOB
	$oRow = mysqli_fetch_assoc($oRecordset);

	//??????,??JSON
	$strJobBegin  = Cal_Start_Key($oRow['strJobBegin'] , $oRow['strJobCount']);
	$strJobCount  = '80000000';
	$strStartTime = date("Y-m-d H:i:s");

	//??SQL,????Job
	$strSQL = "Insert into `JobInfo`(`strJobBegin`,`strJobCount`,`strStartTime`,`iFlagFinish`,`strMemo`) Values( '".$strJobBegin."' , '".$strJobCount."' , '".$strStartTime."' , 0 , ' ')";
	$oRecordset = mysqli_query($connect_db,$strSQL);

	//?????Job
	$strSQL = "SELECT iID,strJobBegin,strJobCount FROM JobInfo where strJobBegin='".$strJobBegin."'";
	$oRecordset = mysqli_query($connect_db,$strSQL);
	if (!$oRecordset)
		return NULL;
	//?????JOB
	$oRow = mysqli_fetch_assoc($oRecordset);

    return $oRow;
}

//???????Job
function Find_Pending_Job($connect_db)
{
	//????????JOBID
	$strSQL = "SELECT iID,strJobBegin,strJobCount,strStartTime FROM JobInfo Where iFlagFinish=0 And date_add(strStartTime, interval 1 day)<now()";
	$oRecordset = mysqli_query($connect_db,$strSQL);
	if (!$oRecordset)
		return NULL;

	//????????JOBID
	$oRow = mysqli_fetch_assoc($oRecordset);
	if (!$oRow['iID'])
		return NULL;

	//??JobID
	$iJobID = $oRow['iID'];

	//????Job???????????????
	$strStartTime = $oRow['strStartTime'];
	$strNow = date("Y-m-d H:i:s");	//????
	if( 0 > strncmp( $strNow , $strStartTime , 19 ) )
		return NULL;

	//??Job?????
	$strSQL = "UPDATE JobInfo SET strStartTime='". $strNow ."' Where iID=". $iJobID;
	$oRecordset = mysqli_query($connect_db,$strSQL);

	//??????
    return $oRow;
}

//echo( 'Start<BR><HR>' );
try
{
	//$mutex = new SyncMutex("UniqueName");
	//$mutex->lock(200);

	//?????
	$connect_db = mysqli_connect('127.0.0.1','root','!QAZxsw2','dbBTC',3306);
	if (!$connect_db)
		return;

	//???????Job
	$PendindJob = Find_Pending_Job($connect_db);
	//???????Job,?????
	if(!$PendindJob)
		$PendindJob = Create_New_Job($connect_db);

	//???????
	mysqli_close($connect_db);

	//$mutex->unlock();

	//??????,??JSON
	$iID          = $PendindJob['iID'];
	$strJobBegin  = $PendindJob['strJobBegin'];
	$strJobCount  = $PendindJob['strJobCount'];
	//$strStartTime = $PendindJob['strStartTime'];
	//$iFlagFinish  = $PendindJob['iFlagFinish'];
	//$strMemo      = $PendindJob['strMemo'];

	$strJSON = "{\"iRspCode\":1000,\"oRspData\":{\"iPrivateCount\":\"".$strJobCount."\",\"szPrivateKey\":\"".$strJobBegin."\",\"iJobID\":".$iID."},\"iRspResult\":0}";
	echo( $strJSON );
}
catch (Exception $e)
{
	echo( 'ERROR<BR>' );
	echo $e->getMessage();
}
?>
