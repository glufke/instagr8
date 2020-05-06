<?
//Get the 5 parameters:
// * s - SESSION (just a number to be used as driving name for files)
// * u - USER (the Instagram username or the Instagram TAG, without #))
// * t - TYPE (this can be "u" for user, or "t" for tag.)
// * n - NUMBER of the post to retrieve.
// * v - VERSION (Being 1 or 2). This is optional.
$ses= htmlspecialchars($_GET["s"])   ;
$t  = htmlspecialchars($_GET["t"])   ;
$us = htmlspecialchars($_GET["u"])   ;
$num= htmlspecialchars($_GET["n"])   ;
$ver= htmlspecialchars($_GET["v"])   ;

//Verify the inputs parameters
if ($ses=='') 
  { echo "ERROR01-Session number is mandatory";
    exit;
  }
if (! is_numeric($ses) )
  { echo "ERROR03-Session should be a number";
    exit;
  }
if ($us =='')
  { echo "ERROR02-User is mandatory";
    exit;
  }

//Create default values
if ($t  =='') $t='u';
if ($num=='' ) $num=0;
else $num=$num-1;
if ($ver=='') $ver=1;

//Open the correct URL depending of the type.
if     ($t=='u') $json = file_get_contents("https://www.instagram.com/$us?__a=1");
elseif ($t=='t') $json = file_get_contents("https://www.instagram.com/explore/tags/$us?__a=1");
else exit;

//Decode JSON response
$arr = json_decode($json, true);

//If type=U, saves some extra info from the USER.
if ($t=='u')
{
  //Create "Name of the User (username)"
  echo $arr["graphql"]["user"]["full_name"] . ' ('.$arr["graphql"]["user"]["username"].')';
  //echo $a.$arr["graphql"]["user"]["profile_pic_url"];
  //echo 'Followed by '. $arr["graphql"]["user"]["edge_followed_by"]["count"].$a;
  //echo 'Following '.   $arr["graphql"]["user"]["edge_follow"]["count"].$a;
} 

//If type=T, saves only the TAG.
if ($t=='t')
{
  echo 'Tag #'. $us.' '; 
}

//SAVE the exact NODE (the number parameter) depending of the type
if ($t=='u') $nod = $arr["graphql"]["user"]["edge_owner_to_timeline_media"]["edges"][$num]["node"]; 
if ($t=='t') $nod = $arr["graphql"]["hashtag"]["edge_hashtag_to_top_posts"]["edges"][$num]["node"];

//Save the Description of the post.
$text =  substr( str_replace( chr(13), ' ', $nod["edge_media_to_caption"]["edges"]["0"]["node"]["text"])     ,0,16*8-1) ;  
echo $text;

//Captures the URL of the IMAGE to be converted
$cmd = 'bash /home/ubuntu/apache/instagr8/instaconv.sh '.$ses.' "'.$nod["thumbnail_src"].'" '.$ver;
$oo = shell_exec($cmd); 

?>
