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
	$iJobID        = $oPost['iJobID'];
	$strPrivateKey = $oPost['strPrivateKey'];
	$iUserID       = $oPost['iUserID'];
	if( 0 == $iJobID )
		return;
	if( '' == $strPrivateKey )
		return;

	//?????
	$connect_db = mysqli_connect('127.0.0.1','root','!QAZxsw2','dbBTC',3306);
	if (!$connect_db)
		return;

	$strSQL = "INSERT INTO JobResult(`iJobID`,`iUserID`,`strPrivate`,`strTime`,`strMemo`) VALUES(". $iJobID . ",". $iUserID . ",'". $strPrivateKey ."',now(),'')";
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