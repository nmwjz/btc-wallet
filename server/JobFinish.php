<?php
//echo( 'Start<BR><HR>' );
try
{
	$oPost = NULL;
	if (empty($_POST))
	{
		$strContent = file_get_contents('php://input');
		$oPost      = (array)json_decode($strContent, true);
	}

	//判断JSON对象是否合法
	if( !oPost )
		return;

	//获取数据
	$iJobID = $oPost['iJobID'];
	if( 0 == $iJobID )
		return;

	//?????
	$connect_db = mysqli_connect('127.0.0.1','root','!QAZxsw2','dbBTC',3306);
	if (!$connect_db)
		return;

	//记录计算完成时间，设置完成状态
	$strFinishTime = date("Y-m-d H:i:s");
	$strSQL = "UPDATE JobInfo set iFlagFinish=1,strFinishTime='". $strFinishTime ."' WHERE iID=".$iJobID;
	$oRecordset = mysqli_query($connect_db,$strSQL);

	//???????
	mysqli_close($connect_db);

	$strJSON = "{\"iRspResult\":0}";
	echo( $strJSON );
}
catch (Exception $e)
{
	echo( 'ERROR<BR>' );
	echo $e->getMessage();
}
?>