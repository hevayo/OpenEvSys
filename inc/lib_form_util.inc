<?php

include_once APPROOT.'inc/lib_form.inc';

function compartmentalize_view_form($entity_form ) {

	$normalFields =array();
	$systemFields = array();
	$supportingDocFields = array();

	foreach( $entity_form as $fieldName => $field){
		 
		if( $entity_form[$fieldName]['type'] != 'submit' ){
			$field_number = $entity_form[$fieldName]['field_number'] % 100 ;
			if( $field_number == 1 || $field_number == 61 || $field_number == 62 || $field_number == 70 || $field_number == 71  ){
				$systemFields[$fieldName] = $field;
			} else if ($field_number == 66){
				$supportingDocFields[$fieldName] = $field;
			}
			else if($fieldName == 'picture' || $fieldName == 'file_size'){
				$systemFields[$fieldName] = $field;
			}
			else{
				$normalFields[$fieldName] = $field  ;
			}
			//echo $fields[$fieldName];
		}
	}

	$compFields = array();
	$compFields[] = $normalFields;
	$compFields[] = $systemFields ;
	$compFields[] = $supportingDocFields;

	return $compFields;
}

function place_form_elements($entity_form, $fields ) {
	$divide_index = divide_index_html_array($entity_form);

	$size = sizeof($fields)/2;
	$counter = 0;
	foreach($entity_form as $fieldName => $field){
		if($entity_form[$fieldName]['type'] != 'submit' ){
			echo $fields[$fieldName];
		}

		if($counter == $divide_index){
		}
		$counter++;
	}
	?>
<br style="clear: both" />
	<?php

	return $fields;
}


function divide_index_html_array( $form_array){

	$htmlLengths = array();
	$totalLength=0;
	foreach($form_array as $fieldName => $formElement){
		$totalLength += get_form_element_length( $formElement['type'] );
		$htmlLengths[ $fieldName ] = $totalLength;
	}
	$divider_length = (int)$totalLength / 2;
	$index = 0;
	foreach($htmlLengths as $length){

		if($length > $divider_length){
			return $index;
		}else{
			$index++;
		}

	}
}

function get_form_element_length($formType){
	switch($formType){
		case 'text':
			return 1;
			break;
		case 'textarea' :
			return 4;
			break;
		case 'mt_select':
			return 1;
			break;
		case 'mt_tree':
			return 1;
			break;
		case 'date':
			return 1;
			break;
		default :
			return 0;
			break;

	}
}



function generate_formarray($entity , $viewType = 'new' , $allVisible = false){
	//$viewType can be 'new' , 'edit' or 'view'
	//var_dump($entity , $viewType);
	global $global;
	if( $entity!=null){
		 
		if($entity == 'victim' || $entity=='perpetrator' || $entity == 'source' || $entity =='intervening_party'){
			//var_dump('generate_form_array',$entity);
			$secEntity = new SecEntity();
			return $secEntity->getFormArray( $entity , $viewType );
		}
		 
		$formArray = array();
		$entityFields = Browse::getEntityFields($entity);

		$visible_field= null;

		$module = shnFrontController::getModule();
		if( $module instanceof eventsModule ){
			if($entity=='event'){
				$record_number = $_GET['eid'];
			}
		}

		switch($viewType){
			case 'new':
				$visible_field = 'visible_new';
				acl_is_entity_allowed($entity , 'create' , $record_number);
				break;
			case 'edit' :
				$visible_field = 'visible_new';  // change to  visible_edit when edit has different forms
				acl_is_entity_allowed($entity , 'update' , $record_number);
				break;
			case 'view':
				$visible_field = 'visible_view';
				//acl_is_entity_allowed($entity , 'read' , $record_number);
				break;
			case 'search':
				$visible_field = 'visible_search';
				break;
			case 'search_view':
				$visible_field = 'visible_search_display';
				break;
			case 'browse':
				$visible_field = 'visible_browse';
				break;
			case 'all':
				$visible_field = null;
				break;
			default :
				$visible_field = 'visible_new';
				break;
		}

		//var_dump($visible_field);
		foreach ( $entityFields as $entityField ){
			$type = $entityField['field_type'];
			$clari = ( (trim($entityField['clar_note']) == 'Y'  || trim($entityField['clar_note']) == 'y' )
			?  true : false );
			$mlt = ( (trim($entityField['is_repeat']) == 'Y'  || trim($entityField['is_repeat']) == 'y' )
			?  true : false );

			$visible = ( $visible_field==null || trim($entityField[$visible_field]) == 'Y'  || trim($entityField[$visible_field]) == 'y' )
			?  true : false ;

			$link_table = $entityField['link_table'];
			$link_field = $entityField['link_field'];

			$field_number = $entityField['field_number'];

			$requiredTerm  =  $entityField['required'] == 'y' ? 'notnull' :  null ;
			$required = $entityField['required'] == 'y' ? true:false;
			$validationString = $entityField['validation'];
			$validation = array();
			$validation = split( ',' , $validationString  );
			if($requiredTerm != null){
				//var_dump($requiredTerm);
				$validation[] = $requiredTerm;
			}


			if($visible == true || $allVisible == true){
				$formArray[  $entityField['field_name'] ]    = array('type'=>$entityField['field_type'],'label'=>$entityField['field_label_l10n'], 'field_number' => $field_number ,
	                            'map'=>array('entity'=>$entity,'field'=>$entityField['field_name'] , 'mt'=>$entityField['list_code'] , 'mlt'=>$mlt , 'link_table'=>$link_table , 'link_field'=>$link_field ),
	                            'extra_opts' => array('clari'=>$clari , 'validation'=>$validation , 'required'=>$required , 'help'=>$entityField['field_number']   )    
				);
			}
		}
		//var_dump($formArray);
		return $formArray;

	}
}

function label_to_field($label){
	$fieldName =    strtr( strtolower( trim($label) ),' ','_' );
	return $fieldName;
}

function popuate_formArray(&$formArray,$entity){
	//var_dump($entity);

	if($formArray != null && $entity!=null){  // The function cannot continue unless the variables have been passed
		foreach($formArray as &$formField){   // iterate throught the elements in the form array

			$recordkeyName = ''. $formField['map']['entity'] . '_record_number';
			$entity_key_value = $entity->$recordkeyName ;

			if($formField['map']['field'] != null && $formField['map']['entity'] == $entity->_table ){   // Field Name should be defined, passed entity should match the entity type in form array
				if($formField['extra_opts']==null){
					$formField['extra_opts'] = array();
				}
				//$formField['extra_opts'] = array();
				if($formField['map']['mlt']=='true'){
					$valueArray= array();
					if(	is_array($entity->$formField['map']['field']) ){
						foreach($entity->$formField['map']['field'] as $vals ){
							$valueArray[] = $vals->vocab_number;
						}
					}
					$formField['extra_opts']['value'] = $valueArray;
				}else{
					$formField['extra_opts']['value'] = $entity->$formField['map']['field'];
				}

				$formField['extra_opts']['clari_value'] = getClariNotes($formField , $entity_key_value);
			}
			else if($formField['map']['field'] == 'picture'){
				$formField['extra_opts']['value'] = $entity->$formField['map']['field'];
			}
			//var_dump($formField['map']['field']);
			//var_dump($formField['extra_opts']);
		}
	}

	//var_dump($formArray);
}

function getClariNotes($formField, $recordNumber){  // pass a form array element and retrive the clarifying notes for the field

	$notesArray = array();

	if( $formField['extra_opts']['clari'] =='true' ){
		if(  $formField['map']['mlt'] == 'true' ){
			$fieldName = $formField['map']['field'] ;
			$clariNote = new ClariNotes();
			$clariNotes = $clariNote->Find( $formField['map']['entity'] , $recordNumber , $formField['map']['field']   );
			foreach($clariNotes as $note){
				//var_dump( $note->vocab_number, $note->value );
				$notesArray[ "$note->vocab_number" ] = $note->value ;
			}

		}else{
			$clariNote = new ClariNotes();
			$clariNotes = $clariNote->Find( $formField['map']['entity'] , $recordNumber , $formField['map']['field']   );
			if(sizeof($clariNotes) > 0 ){
				$note = $clariNotes[0];

				//var_dump( $note->vocab_number, $note->value );

				$notesArray[ "$note->vocab_number" ] = $note->value ;
			}
		}
	}
	//var_dump($notesArray);
	return $notesArray;
}


function form_objects($formArray,&$entity, $valuesArray=null) {
	if($valuesArray == null){                // testing
		$valuesArray = $_POST;
	}
	//var_dump($valuesArray);
	//$formArrayEntry = current($formArray);
	//reset($formArray);
	$recordkeyName = ''. $entity->_table . '_record_number';
	//$entity_key_value = $entity->$recordkeyName;

	if($entity->$recordkeyName == null && ($valuesArray[$recordkeyName] == null || $valuesArray[$recordkeyName] == '' ) ){
		$entity_key_value = $entity->$recordkeyName =  shn_create_uuid($entity->_table);
	}else if($entity->$recordkeyName != null ){
		$entity_key_value = $entity->$recordkeyName;
	}else{
		$entity_key_value = $entity->$recordkeyName = $valuesArray[$recordkeyName];
	}
	if( $entity_key_value == null  ){
		throw new Invalide_Parameter_Exception("WTF!!! add a value for $recordkeyName ");
	}

	$entity->clari_notes = array();

	foreach ( $formArray as $fieldName=>$formField){
		if( $formField['map']['entity'] == $entity->_table  ){

			$recordkeyName = ''. $formField['map']['entity'] . '_record_number';            //have to change to handle act's child tables which have the act_record_number
			if($formField['map']['field'] == $recordkeyName){
				if($valuesArray[ $fieldName] !=''){
					$entity->LoadFromRecordNumber($valuesArray[ $fieldName]);
					$entity->LoadManagementData();
					$entity_key_value =  $valuesArray[ $fieldName] ;
					//var_dump('values array field name ' , $valuesArray[ $fieldName]);
				}
			}


			//var_dump($formField['map']);
			if($formField['map']['mlt'] != 'true'){  // non multivalue terms
				//$name = $formField['map']['field'];
				if ( $formField['map']['mt'] != '0'){   // micro thesauri terms
					if (trim($valuesArray[$fieldName])!='' ){       // blank selections should be ignored
						if(!($valuesArray[ $fieldName] == null || trim($valuesArray[ $fieldName]) == '')){
							$entity->$formField['map']['field'] = $valuesArray[ $fieldName];
						}else{
							$entity->$formField['map']['field'] = null;
						}
					}
				}else {
					if( $fieldName != $recordkeyName ){

						if(!($valuesArray[ $fieldName] == null && $fieldName == 'supporting_documents')){
							$entity->$formField['map']['field'] = $valuesArray[ $fieldName];
						}
						else if(!($valuesArray[ $fieldName] == null || trim($valuesArray[ $fieldName]) == '')){
							$entity->$formField['map']['field'] = $valuesArray[ $fieldName];
						}else{
							$entity->$formField['map']['field'] = null;
						}
					}
				}


			}else{                                  // multivalue Terms
				$termArray =  array();
				if($valuesArray[$fieldName]!=null){
					if(is_array($valuesArray[$fieldName] )){
						$valuesArray[$fieldName] = array_unique($valuesArray[$fieldName] ); //remove duplicate MT values
						foreach($valuesArray[$fieldName] as $mtvalue){
							if (trim($mtvalue)!='' ){
								$mtTerm = MtFieldWrapper::getMTObject($formField['map']['field'] , $formField['map']['entity']);
								$mtTerm->vocab_number = $mtvalue;
								$mtTerm->record_number = $entity->$recordkeyName ;
								$termArray[] =  $mtTerm;
							}
						}
					}else{
						if (trim($valuesArray[$fieldName])!='' ){
							$mtTerm = MtFieldWrapper::getMTObject($formField['map']['field'] , $formField['map']['entity']);
							$mtTerm->vocab_number = $valuesArray[$fieldName];
							$mtTerm->record_number = $entity->$recordkeyName ;
							$termArray[] =  $mtTerm;
						}

					}
				}
				//var_dump($termArray);
				if( sizeof($termArray) > 0 ){
					$entity->$formField['map']['field'] = $termArray;
				}
			}

			if($formField['extra_opts']['clari'] == 'true'){  // Clarifying Notes



				if($formField['map']['mlt'] != 'true'){              //NON multi value
					/*
					$clari_note_object = new ClariNotes();
					$clari_note_object->entity = $entity->_table ;
					$clari_note_object->record_number = $entity_key_value;
					$clari_note_object->field_name = $formField['map']['field'];
					$clari_note_object->value = $valuesArray[$fieldName.'_clarify']; */
					$clariNote = ClariNotes::getClariNoteObject($entity->_table , $entity_key_value ,
					$formField['map']['field'],$valuesArray[$fieldName],$valuesArray[$fieldName.'_clarify'] );
					if($clariNote->value != null){
						$entity->clari_notes[] = $clariNote;
					}

				}else{
					if( is_array( $valuesArray[$fieldName] ) )
					foreach($valuesArray[$fieldName] as $mtvalue){
						/*
						 $clari_note_object = new ClariNotes();
						 $clari_note_object->entity = $entity->_table ;
						 $clari_note_object->record_number = $entity_key_value;
						 $clari_note_object->field_name = $formField['map']['field'];
						 $clari_note_object->vocab_number = $mtvalue; //iterate this array to get clari notes for each multi value
						 $clari_note_object->value = $valuesArray[$fieldName.'_clarify_' . $mtvalue ]; */
						 
						$clariNote = ClariNotes::getClariNoteObject($entity->_table , $entity_key_value ,
						$formField['map']['field'],$mtvalue ,$valuesArray[ $fieldName .  '_' . $mtvalue . '_clarify'  ] );
						 
						 
						if($clariNote->value != null){
							$entity->clari_notes[] = $clariNote;
						}
					}
				}
			}
		}
	}
	//var_dump($entity);

	return $entity;
}




function shn_form_validate(&$form)
{

	//set_include_path( get_include_path() . ':'. APPROOT .'3rd/');

	//require_once "Zend/Loader.php";
	//Zend_Loader::registerAutoload();

	include_once APPROOT.'inc/lib_validate.inc';
	$valide  = true ;



	foreach($form as $field_id => &$field){
		$field['extra_opts']['error'] = array();

		$pkField = get_primary_key( $field['map']['entity'] );
		if( $pkField != $field['map']['field']){

			$vals = $_POST[$field_id];
			$valFunctions = $field['extra_opts']['validation'];
			if( is_array($valFunctions) && sizeof($valFunctions)>0  ){
				foreach($valFunctions as $function){
					if($function!=''){
						//var_dump('validating ' , $vals , $function);
						//$ok = Zend_Validate::is($vals, $function);
						//var_dump($ok);
						$valMsg = null;
						$ok = validate_field($function, $vals, $valMsg);
						//var_dump($function  , $ok);
						 
						if( ! ($ok == true ) ){
							$valide = false;
							$field['extra_opts']['error'][] = $valMsg;
							//var_dump($ok);
						}
					}
				}
			}
			/*
			 $validate = 'shn_valid_'.$field['valid'];
			 $error = true;
			 if(isset($vals)&& function_exists($validate)){
			 if(is_array($vals)){
			 //validate all elements
			 foreach($vals as $value)
			 $error = $validate($_POST[$field_id]) && $error;
			 }
			 else
			 $error = $validate($_POST[$field_id]);
			 if(!$error){
			 $eo = $form[$field_id]['extra_opts'];
			 $eo=(is_array($eo))?$eo:array();
			 $eo['error']=true;
			 $form[$field_id]['extra_opts'] = $eo;
			 $gerror = false;
			 }
			 }
			 */
		}
	}

	//spl_autoload_register(array('AutoLoadClass', '__autoload'));

	//var_dump($form);
	return $valide;
}

function validate_field($validationType , $vals , &$errorMsg){
	$validate = 'shn_valid_'.$validationType ;
	$valid = true;
	//hack to array values
	if($validationType == 'notnull' && !isset($vals)){
		$valid = $validate($vals) && $valid;
		if(!$valid)
		$errorMsg = shn_valid_get_error();
	}

	if(isset($vals)&& function_exists($validate)){
		if(is_array($vals)){
			//validate all elements
			foreach($vals as $value)
			$valid = $validate($value ) && $valid;
		}
		else
		$valid = $validate($vals) && $valid;
		if(!$valid){
			$errorMsg = shn_valid_get_error();
			//var_dump($errorMsg);
		}
	}

	return $valid;


}

function shn_form_get_html_fields($form , $field_div = true )
{
	$i = 0;
	$fields = array();
	foreach($form as $field_id => $options){
		$controler = $options['type'];
		$controler = 'shn_form_'.$controler;
		$controler .= (isset($options['map']['mlt'])&&$options['map']['mlt']==true)?'_mlt':'';
		if(function_exists($controler)){
			$options['extra_opts']['mt']=$options['map']['mt'];
			ob_start();
			$controler($options['label'],$field_id,$options['extra_opts']);
			$html = ob_get_clean();
			if($options['type']!='submit' && $field_div)
			$html = "<div class='field'>$html</div>";

			$fields[$field_id] = $html;
			$fields[$i++]= $html;
		}
		else
		;
		// echo $controler._t('_CONTROLER_NOT_FOUND_');
	}

	return $fields;
}

function shn_form_get_html_table($columnNames = null, $columnValues = null)
{
	echo "<table class='browse'>";
	echo "<thead>";
	echo "<tr>";
	foreach($columnNames as $field=>$value){
		$url = get_sort_heading($field);

		$columnId = get_column_sort_id($field);

		$columnClass = null;
		if($columnId!=null){
			$columnClass = 'id="'. $columnId . '"';
		}

		echo '<td '. $columnClass .'> <a href="' . $url  .'">' . $value ."</a></td>";
	}
	echo "</tr>";
	echo "</thead>";
	echo "<tbody>";

	$i=0;
	foreach($columnValues as $columnvalue){
		?>
<tr <?php echo ($i++%2==1)?'class="odd"':''; ?>>
<?php
$tdIndex = 1;
foreach($columnNames as $field=>$value){
	if($tdIndex == 1){
		if($columnvalue[$field] == null || trim($columnvalue[$field]) == ''){
			$columnvalue[$field] = " . . . ";
		}
		$displayValue = '<a href="' . $columnvalue['record_url']  . '">' . $columnvalue[$field] . '</a>';
	}else{
		$displayValue = $columnvalue[$field];
	}

	echo "<td>" . $displayValue ."</td>";
	$tdIndex++;
}
echo "</tr>";
	}

	echo "</tbody>";
	echo "</table>";
}

function shn_form_get_html_filter_table($columnNames = null, $columnValues = null , $htmlFilterFields = null)
{
	$requestObj = new shnRequest();
	$mod = $requestObj->getRequestModule();
	$action = $requestObj->getRequestAction();
	$cols = count($columnNames);

	?>

	<form action="<?php get_url($mod, $action)?>" method="get">
		<input type="hidden" name="mod" value="<?php echo $mod?>" /> <input
			type="hidden" name="act" value="<?php echo $action?>" />

		<table class='browse'>
			<thead>
				<tr class='top_but'>
					<td colspan='<?php echo $cols?>'><input type='submit' name="filter"
						value='<?php echo _t('FILTER')?>' /><span>&nbsp;</span> <a
						href="<?php get_url($mod, $action,null,array('reset'=>'true')); ?>"><?php echo _t('RESET') ?>
					</a>
					</td>
				
				
				<tr>
				
				
				<tr class="filter" id="filter_text">
				<?php
					
				foreach($columnNames as $field=>$value){
					$label = ucwords(str_replace('_',' ', $field));
					echo "<td id='td_".$field."'>"."<span id='span_".$field."' class='inactive'>".$value."</span>" . $htmlFilterFields[$field] ."</td>";
				}

				?>
				</tr>
				<?php
				echo "<tr>";
				foreach($columnNames as $field=>$value){
					$url = get_sort_heading($field);

					$columnId = get_column_sort_id($field);

					$columnClass = null;
					if($columnId!=null){
						$columnClass = 'id="'. $columnId . '"';
					}

					echo '<td '. $columnClass .'> <a href="' . $url  .'">' . $value ."</a></td>";
				}
				echo "</tr>";

				echo "</thead>";
				if($columnValues != null && count($columnValues) != 0){
					echo "<tbody>";

					$i=0;
					foreach($columnValues as $columnvalue){
						?>
				<tr <?php echo ($i++%2==1)?'class="odd"':''; ?>>
				<?php
				$tdIndex = 1;

				foreach($columnNames as $field=>$value){
					if($tdIndex == 1){
						if($columnvalue[$field] == null || trim($columnvalue[$field]) == ''){
							$columnvalue[$field] = " . . . ";
						}
						$displayValue = '<a href="' . $columnvalue['record_url']  . '">' . $columnvalue[$field] . '</a>';
					}
					else if($tdIndex == 2 && $columnvalue['entity_record_url'] != null){
						if($columnvalue[$field] == null || trim($columnvalue[$field]) == ''){
							$columnvalue[$field] = " . . . ";
						}
						$displayValue = '<a href="' . $columnvalue['entity_record_url']  . '" title="'.$columnvalue['record_number'].'">' . $columnvalue[$field] . '</a>';
					}
					else{
						$displayValue = $columnvalue[$field];
					}

					echo "<td>" . $displayValue ."</td>";
					$tdIndex++;
				}

				echo "</tr>";
					}

					echo "</tbody>";
				}
				else{
					shnMessageQueue::addInformation(_t('NO_RECORDS_WERE_FOUND_'));
					echo shnMessageQueue::renderMessages();
				}

				?>
		
		</table>
	</form>
	<?php
}

function get_sort_heading($field){
	$newGet = $_GET;

	if( $newGet['sort'] != $field ){
		unset($newGet['sort']);
	}

	if(!isset($newGet['sort'] )){
		$newGet['sortorder'] = 'asc';
	}else{
		if($newGet['sortorder'] == 'asc')
		$newGet['sortorder'] = 'desc';
		else
		$newGet['sortorder'] = 'asc';

	}


	$newGet['sort'] = $field;
	$url = get_url($_GET['mod'], $_GET['act'] , $_GET['view'] ,$newGet , null, true);


	return $url;
}

function get_column_sort_id($field){
	if(isset($_GET['sort'])){
		if( $_GET['sort'] == $field  ){
			if( $_GET['sortorder'] == 'asc' ){
				return 'column_asc';
			}else if($_GET['sortorder'] == 'desc'){
				return 'column_desc';
			}
		}

	}
	return null;
}

function shn_form_get_html_labels($form, $return = true)
{
	include_once(APPROOT.'inc/lib_view.inc');
	$i = 0;
	$fields = array();
	ob_start();

	$compForm = compartmentalize_view_form($form);
	$classNumber  = 2;
	foreach($compForm as $form){

		echo "<table class='view$classNumber'>";
		$classNumber++;
		foreach($form as $field_id => $options){

			if($options['map']['link_table'] != null ){
				$options['extra_opts']['value'] .= '<b>' . '  :  ' . loadLinkName( $options['map']['link_table'] , $options['map']['link_field'] ,  $options['extra_opts']['value']  ) .'</b>';
			}

			$controler = $options['type'];
			$controler = 'shn_view_'.$controler;
			$controler .= (isset($options['map']['mlt'])&&$options['map']['mlt']==true)?'_mlt':'';
			if(function_exists($controler)){
				$options['extra_opts']['mt']=$options['map']['mt'];
				$options['extra_opts']['class'] = ($i++%2==1)?'odd':'even';
				$controler($options['label'],$options['extra_opts']['value'],$options['extra_opts']);
			}

		}
		echo "</table>";
	}
	echo "<div style='clear:both' ></div>";

	$html = ob_get_clean();
	if($return){
		$key = array_keys($form);
		return array($key[0]=>$html);
	}
	else
	echo $html;
}

function loadLinkName( $entity, $link_field , $record_number ){
	switch($entity){
		case 'event':
			$entity = new Event();
			break;
		case 'person' :
			$entity = new Person();
			break;
	}

	$entity->LoadfromRecordNumber($record_number);
	return $entity->$link_field;

}


function shn_form_get_html_doc_search_ctrl($columnNames = null, $columnValues = null , $htmlFilterFields = null)
{
	?>
	<table class="browse">
		<thead>
		<?php

		$count = 1;
		$no_of_column = 1;
		foreach($columnValues as $columnvalue ){
			if($count == 1){
				echo "<tr>";
				foreach($columnNames as $field=>$value){
					echo "<td>$value</td>";
					$no_of_column ++;
				}
				echo "<td>" ._t('ACTION')."</td>";
				echo "</tr>";
			}
			$count ++;

		}
		?>
		</thead>
		<tbody>
		<?php


		$i=0;

		foreach($columnValues as $columnvalue){
			?>
			<tr <?php echo ($i++%2==1)?'class="odd"':''; ?>>
			<?php

			$tdIndex = 1;
			foreach($columnNames as $field=>$value){
				if($tdIndex == 1){
					$displayValue = '<a target="_blank" href="' . $columnvalue['record_url']  . '">' . $columnvalue[$field] . '</a>';
				}else{
					$displayValue = $columnvalue[$field];
				}
				echo "<td>" . $displayValue . "</td>";
				$tdIndex++;
			}
			?>
				<a target="_blank"></a>
				<td><input type="checkbox" name="doc_id_list[]"
					id="<?php echo $columnvalue['doc_id']?>"
					value="<?php echo $columnvalue['doc_id'] .'_'.$columnvalue['title'];?>" />
				</td>
			</tr>
			<?php
		}
		?>
		</tbody>
		<tr>
			<td colspan="<?php echo $no_of_column;?>" align="right"><input
				type="submit" value="select" id="selected_doc" name="selected_doc"
				onclick="selected_doclist(this.form,'document_field'); return false;" />
			</td>
		</tr>
	</table>
	<?php
}

function shn_form_get_html_person_search_ctrl($columnNames = null, $columnValues = null , $htmlFilterFields = null, $mod=null, $act=null)
{
	?>
	<table class="browse">
		<thead>
		<?php

		$count = 1;
		foreach($columnValues as $columnvalue ){
			if($count == 1){
				echo "<tr>";
				foreach($columnNames as $field=>$value){
					echo "<td>$value</td>";
				}
				echo "<td>" ._t('ACTION')."</td>";
				echo "</tr>";
			}
			$count ++;
		}
		?>
		</thead>
		<tbody>
		<?php


		$i=0;

		foreach($columnValues as $columnvalue){
			?>
			<tr <?php echo ($i++%2==1)?'class="odd"':''; ?>>
			<?php

			$tdIndex = 1;
			foreach($columnNames as $field=>$value){
				if($tdIndex == 1){
					$displayValue = '<a target="_blank" target="_blank" href="' . $columnvalue['record_url']  . '">' . $columnvalue[$field] . '</a>';
				}else{
					$displayValue = $columnvalue[$field];
				}
				echo "<td>" . $displayValue . "</td>";
				$tdIndex++;
			}
			?>
				<td><a
					href="<?php echo get_url($mod,$act,null,array('information_id'=>$_GET['information_id'], 'intervention_id'=>$_GET['intervention_id'], 'person_id'=>$columnvalue['person_record_number'])); ?>"><?php echo _t('SELECT'); ?>
				</a></td>
			</tr>
			<?php
		}
		?>
		</tbody>
	</table>
	<?php
}

function shn_form_get_html_event_search_ctrl($columnNames = null, $columnValues = null , $htmlFilterFields = null, $mod=null, $act=null)
{
	?>
	<table class="browse">
		<thead>
		<?php

		$count = 1;
		foreach($columnValues as $columnvalue ){
			if($count == 1){
				echo "<tr>";
				foreach($columnNames as $field=>$value){
					echo "<td>$value</td>";
				}
				echo "<td>" ._t('ACTION')."</td>";
				echo "</tr>";
			}
			$count ++;
		}
		?>
		</thead>
		<tbody>
		<?php


		$i=0;

		foreach($columnValues as $columnvalue){
			?>
			<tr <?php echo ($i++%2==1)?'class="odd"':''; ?>>
			<?php

			$tdIndex = 1;
			foreach($columnNames as $field=>$value){
				if($tdIndex == 1){
					$displayValue = '<a target="_blank" href="' . $columnvalue['record_url']  . '">' . $columnvalue[$field] . '</a>';
				}else{
					$displayValue = $columnvalue[$field];
				}
				echo "<td>" . $displayValue . "</td>";
				$tdIndex++;
			}
			?>
				<td><a
					href="<?php echo get_url($mod,$act,null,array('event_id'=>$columnvalue['event_record_number'], 'coeid'=>$_GET['coeid'])); ?>"><?php echo _t('SELECT'); ?>
				</a></td>
			</tr>
			<?php
		}
		?>
		</tbody>
	</table>
	<?php
}