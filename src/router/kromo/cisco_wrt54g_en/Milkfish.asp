<% do_pagehead("bmenu.servicesMilkfish"); %>
		<script type="text/javascript">
		//<![CDATA[

function to_submit(F) {
	F.change_action.value = "";
	F.submit_type.value = "";
	F.save_button.value = sbutton.saving;
	checked(F);
	apply(F);
}

var update;

addEvent(window, "load", function() {

		show_layer_ext(document.setup.milkfish_enabled, 'idmilkfish', <% nvram_else_match("milkfish_enabled", "1", "1", "0"); %> == 1);
		show_layer_ext(document.setup.milkfish_dynsip, 'idmilkfish_dynsip', <% nvram_else_match("milkfish_dynsip", "1", "1", "0"); %> == 1);

		update = new StatusbarUpdate();
		update.start();
		
});

addEvent(window, "unload", function() {
	update.stop();

});		
		//]]>
		</script>
	</head>

	<body class="gui">
		<% showad(); %>
		<div id="wrapper">
			<div id="content">
				<div id="header">
					<div id="logo"><h1><% show_control(); %></h1></div>
					<% do_menu("Services.asp","Milkfish.asp"); %>
				</div>
				<div id="main">
					<div id="contents">
						<form name="setup" action="applyuser.cgi" method="<% get_http_method(); %>">
							<input type="hidden" name="submit_button" value="Milkfish" />
							<input type="hidden" name="action" value="Apply" />
							<input type="hidden" name="change_action" value="gozila_cgi" />
							<input type="hidden" name="submit_type" />
							<input type="hidden" name="commit" value="1" />
							
	<h2><% tran("bmenu.servicesMilkfish"); %></h2>

	<fieldset>
		<legend><% tran("bmenu.servicesMilkfish"); %></legend>
			<div class="setting">
			<div class="label">Milkfish Service</div>
				<input class="spaceradio" type="radio" name="milkfish_enabled" value="1" <% nvram_checked("milkfish_enabled", "1"); %> onclick="show_layer_ext(this, 'idmilkfish', true)" /><% tran("share.enable"); %>&nbsp;
				<input class="spaceradio" type="radio" name="milkfish_enabled" value="0" <% nvram_checked("milkfish_enabled", "0"); %> onclick="show_layer_ext(this, 'idmilkfish', false)" /><% tran("share.disable"); %>
			</div>
<div id="idmilkfish">
		<div class="setting">
			<div class="label">Username</div>
			<input size="27" name="milkfish_username" value="<% nvram_get("milkfish_username"); %>" />
		</div>
		<div class="setting">
			<div class="label">Password</div>
			<input size="27" name="milkfish_password" value="<% nvram_get("milkfish_password"); %>" />
		</div>
		<div class="setting">
			<div class="label">From-Substitution</div>
			<input class="spaceradio" type="radio" name="milkfish_fromswitch" value="1" <% nvram_checked("milkfish_fromswitch", "1"); %> /><% tran("share.enable"); %>&nbsp;
			<input class="spaceradio" type="radio" name="milkfish_fromswitch" value="0" <% nvram_checked("milkfish_fromswitch", "0"); %> /><% tran("share.disable"); %>
		</div>
		<div class="setting">
			<div class="label">From-Domain</div>
			<input size="27" name="milkfish_fromdomain" value="<% nvram_get("milkfish_fromdomain"); %>" />
		</div>
		<div class="setting">
			<div class="label">Home SIP</div>
			<input class="spaceradio" type="radio" name="milkfish_dynsip" value="1" <% nvram_checked("milkfish_dynsip", "1"); %> onclick="show_layer_ext(this, 'idmilkfish_dynsip', true)" /><% tran("share.enable"); %>&nbsp;
			<input class="spaceradio" type="radio" name="milkfish_dynsip" value="0" <% nvram_checked("milkfish_dynsip", "0"); %> onclick="show_layer_ext(this, 'idmilkfish_dynsip', false)" /><% tran("share.disable"); %>
		</div>
		<div id="idmilkfish_dynsip">
		<div class="setting">
			<div class="label">HomeSIP Address</div>
			<input size="27" name="milkfish_homesipaddr" value="<% nvram_get("milkfish_homesipaddr"); %>" />
		</div>
		</div>
		<div class="setting">
			<div class="label">Milkfish Audit</div>
			<input class="spaceradio" type="radio" name="milkfish_audit" value="1" <% nvram_checked("milkfish_audit", "1"); %> /><% tran("share.enable"); %>&nbsp;
			<input class="spaceradio" type="radio" name="milkfish_audit" value="0" <% nvram_checked("milkfish_audit", "0"); %> /><% tran("share.disable"); %>
		</div>

	<br />

	<fieldset>
		<legend>SIP Phonebook</legend>

	</fieldset>
	
<br />



	<fieldset>
		<legend>SIP Database</legend>

	</fieldset>
	
<br />



	<fieldset>
		<legend>SIP Messaging</legend>

	</fieldset>
	
<br />

</div>
</fieldset>
<br/>

	<h2>Milkfish Status</h2>

	<fieldset>
		<legend>Current Status</legend>

	</fieldset>
	<br />
							
							<div class="submitFooter">
								<script type="text/javascript">
								//<![CDATA[
								submitFooterButton(1,1);
								//]]>
								</script>
							</div>
						</form>
					</div>
				</div>
				<div id="helpContainer">
					<div id="help">
						<div><h2><% tran("share.help"); %></h2></div>
						<br/>
						<a href="javascript:openHelpWindow<% nvram_selmatch("dist_type","micro","Ext"); %>('HMilkfish.asp');"><% tran("share.more"); %></a>
					</div>
				</div>
				<div id="floatKiller"></div>
				<div id="statusInfo">
				<div class="info"><% tran("share.firmware"); %>: 
					<script type="text/javascript">
					//<![CDATA[
					document.write("<a title=\"" + share.about + "\" href=\"javascript:openAboutWindow()\"><% get_firmware_version(); %></a>");
					//]]>
					</script>
				</div>
				<div class="info"><% tran("share.time"); %>:  <span id="uptime"><% get_uptime(); %></span></div>
				<div class="info">WAN<span id="ipinfo"><% show_wanipinfo(); %></span></div>
				</div>
			</div>
		</div>
	</body>
</html>