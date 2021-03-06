<?php global $conf; ?>

<div class='panel'>

    <div class="form-container"> 
        <h2><?php echo _t('Google Authenticator') ?></h2>

        <form class="form-horizontal"  action='<?php echo get_url('home', 'edit_security') ?>' method='post'>
            <?php if ($enabled) {
                if($changed){
                    ?>
             <div class="alert alert-success">
                        <div><b><?php echo _t('Enabled successfully.') ?></b></div>
                    </div>
            <?php
                }
                ?>
                
                        <button type="submit" class="btn" name="disable" ><i class="icon-remove-circle"></i> <?php echo _t('Disable') ?></button>
                   
                <?php
            } else {
                ?>

                
                <div class="control-group">
                    <p><?php echo _t('Install the Google Authenticator app for your phone') ?></p>
                    <ol class="ol p10">
                        <li><?php echo _t('On your phone, open a web browser.') ?></li>
                        <li><?php echo _t('Go to m.google.com/authenticator.') ?></li>
                        <li><?php echo _t('Download and install the Google Authenticator application.') ?></li>
                    </ol>
                    <p><?php echo _t('Now open and configure Google Authenticator.') ?></p>
                    <br /><p><?php echo _t('Scan following Barcode to register the application automaticly:') ?><p>
                    <div>
                        <img src="<?php echo $url ?>" width="100" height="100" />
                    </div>
                    <p><?php echo _t('Or use the following secret key to register the application manually:') ?></p>
                    <div class="alert alert-info">
                        <div><b><?php echo $secret ?></b></div>
                    </div><br />
                    <p><?php echo _t('Once you manually entered and saved your key, enter the 6-digit verification code generated by the Authenticator app.') ?> </p>
                </div>
                <div class='control-group <?php if ($wrongcode) {
                echo ' error';
            } ?>'>	
                    <label  class="control-label" for="code"><?php echo _t('Code') ?></label>

                    <div class="controls">
                        <input  type="text" name="code"  value=""   class='input-large <?php if ($wrongcode) {
                echo ' error';
            } ?>'  />
                        <div class="help-inline">
                            <span class="label label-important"><?php echo _t('IS_REQUIRED') ?></span>  
    <?php if ($wrongcode) { ?> <span class="help-inline"><?php echo _t('The code is incorrect. Try again') ?></span><?php } ?>
                        </div>
                    </div>
                </div>
                <div class="control-group">
                    <div class="controls">

                        <button type="submit" class="btn btn-primary" name="save" ><i class="icon-ok icon-white"></i> <?php echo _t('SAVE') ?></button>
                    </div></div>
    <?php
}
?>
        </form>
    </div>
</div>

