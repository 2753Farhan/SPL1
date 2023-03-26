<?php 

	$success = "nothing";

	if(isset($_POST['submit'])){
		include 'includes/db-config.php';
		
		$psswd = md5($_POST["password"]);
		$query = "INSERT INTO users values ('" . $_POST['email'] . "', '" .
												 $_POST['name'] . "', '" .
												 $psswd . "', 'Student')";
		// echo $query;
		
		$sqlResult = mysqli_query($sqlConnect,$query);
		
		if($sqlResult){
			$success = "true";
		}
		else {
			$success = "false";
			echo $psswd;
		}
	}
?>


<!DOCTYPE html>
<html>

<head>
	<?php include 'includes/header.php'; ?>
	<script type="text/javascript" src="js/registration.js"></script>
	<title>Register</title>
</head>

<body>
	<?php include 'includes/topmenu.php'; ?>
	<div class="container-fluid" id="main">
		
		<div class="row" id="content">
			<div class="col-md-2" id="leftPanel">
				<p style="color:white">a</p>
			</div>
			
			<div class="col-md-8" id="writing">
				<style>
					.register-form form.custom-form{
						box-sizing:border-box;
						background-color:#ffffff;
						box-shadow:0 1px 3px 0 rgba(0, 0, 0, 0.1);
						font:bold 14px sans-serif;
						text-align:center;
						margin:20px;
						color:#333;
					}

					@media (max-width:400px) {
						.register-form form.custom-form{
							padding:55px 10px;
						}
					}

					.register-form .custom-form h1{
						display:inline-block;
						color:#4c565e;
						font-size:24px;
						font-weight:bold;
						padding:0 10px 15px;
						margin-bottom:60px;
						border-bottom:2px solid rgb(108, 174, 224);
					}

					.register-form .custom-form .form-group{
						margin-bottom:25px;
					}

					.register-form .custom-form .label-column{
						text-align:right;
						color:#5F5F5F;
					}

					@media (max-width:768px) {
						.register-form .custom-form .label-column{
							text-align:left;
						}
					}

					.register-form .custom-form .input-column{
						color:#5f5f5f;
						text-align:left;
					}

					.register-form .custom-form .input-column input{
						color:#5f5f5f;
						box-shadow:1px 2px 4px 0 rgba(0, 0, 0, 0.08);
						padding:12px;
						border:1px solid #dbdbdb;
						border-radius:2px;
						height:42px;
					}

					.register-form .custom-form .dropdown .dropdown-toggle{
						background:#fff;
						border:1px solid #dbdbdb;
						box-shadow:1px 2px 4px 0 rgba(0, 0, 0, 0.08);
						color:#333;
						outline:none;
					}

					.register-form .custom-form .dropdown ul{
						background:#fff;
					}

					.register-form .custom-form .dropdown ul li a{
						background:#fff;
						color:#333;
						opacity:0.8;
					}

					.register-form .custom-form .dropdown ul li a:hover{
						opacity:1;
					}

					.register-form .custom-form .submit-button{
						border-radius:2px;
						background:#6caee0;
						color:#ffffff;
						font-weight:bold;
						box-shadow:1px 2px 4px 0 rgba(0, 0, 0, 0.08);
						padding:14px 22px;
						border:0;
						margin:30px;
						outline:none;
					}
				</style>

				<div class="register-form">
					<form name="registrationForm" class="form-horizontal custom-form" action="register.php" method="post" onSubmit="return formValidation()">
						<h1>Registration Form</h1>

						<?php
							if($success == "false"){ ?>
								<div class="alert alert-danger">
								<strong>Error!</strong> That email already exists. Please sign in or register with another email.
								</div>
							<?php } else if ($success == "true"){ ?>
								<div class="alert alert-success">
								<strong>Success!</strong> <span style="font-weight:normal">Registration successful. <a href="login.php">Login</a>. </span>
								</div>
						<?php } ?>

						<div class="form-group">
							<div class="col-sm-4 label-column">
								<label class="control-label" for="name-input-field">Name </label>
							</div>
							<div class="col-sm-6 input-column">
								<input class="form-control" name="name" type="text" required>
							</div>
						</div>
						<div class="form-group">
							<div class="col-sm-4 label-column">
								<label class="control-label" for="email-input-field">Email </label>
							</div>
							<div class="col-sm-6 input-column">
								<input class="form-control" name="email" type="email" required>
							</div>
						</div>
						<div class="form-group">
							<div class="col-sm-4 label-column">
								<label class="control-label" for="pawssword-input-field">Password </label>
							</div>
							<div class="col-sm-6 input-column">
								<input class="form-control" name="password" type="password" required>
							</div>
						</div>
						<div class="form-group">
							<div class="col-sm-4 label-column">
								<label class="control-label" for="repeat-pawssword-input-field">Repeat Password </label>
							</div>
							<div class="col-sm-6 input-column">
								<input class="form-control" name="confirmPassword" type="password" required>
							</div>
						</div>
						<div class="checkbox">
							<label>
								<input type="checkbox" required>I've read and accept the <a href="terms.php">terms and conditions</a>
							</label>
						</div>
						<button class="btn btn-default submit-button" name="submit" type="submit" onClick="return formValidation()">Submit Form</button>
					</form>
					</div>
				</div>
			</div>
			
			<div class="col-md-2" id="rightPanel">
				<p style="color:white">a</p>
			</div>
    	</div>
	</div>
</body>

</html>


