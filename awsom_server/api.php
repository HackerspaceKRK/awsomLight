<?php
class Light{
	private $db;
	function __construct(){
		$this->db = new SQLite3('light.db');
	}
	function __destruct(){
		$this->db -> close();
	}

	function getAll(){
		$q = false;
		while($q === false)
			$q = $this->db -> query('SELECT state FROM light ORDER BY channel ASC');
		$v = '';
		while($r = $q->fetchArray()){
			$v .= $r['state'];
		}
		return $v;
	}
	function getIfAny(){
		$q = $this->db -> query('SELECT count(*) as count FROM light WHERE description != \'empty\' AND state = 1');
		if($q !== false and $r = $q->fetchArray())
			return ($r['count'] > 0 ? '1' : '0');
		return '0';
	}

	function setAll($state){
                $q = $this->db->prepare('UPDATE light SET state=:state WHERE description != \'empty\'');
                $q->bindValue(':state', $state, SQLITE3_INTEGER);
                if($q === false)
                        return '?';
                $r=$q->execute();
                if($r === false)
                        return '?';
                return $this->getAll();

	}

	function toggleByDescription($description){
		$s = $this->getByDescription($description);
		switch($s){
			case '1':
				return $this->setByDescription($description, 0);
				break;

			case '0':
				return $this->setByDescription($description, 1);
				break;
			default:
				return $s;
		}
	}

	function toggleByChannel($channel){
		$s = $this->getByChannel($channel);
		switch($s){
			case '1':
				return $this->setByChannel($channel, 0);
				break;

			case '0':
				return $this->setByChannel($channel, 1);
				break;
			default:
				return $s;
		}
	}

	function getByDescription($description){
		$q = $this->db->prepare('SELECT state FROM light WHERE description=:description');
		$q->bindValue(':description', $description, SQLITE3_TEXT);
		if($q === false)
			return '?';
		$r=$q->execute();
		if($r === false)
			return '?';
		$r=$r->fetchArray();
		if($r === false)
			return '?';
		return $r['state'];
	}

	function getByChannel($channel){
		$q = $this->db->prepare('SELECT state FROM light WHERE channel=:channel');
		$q->bindValue(':channel', $channel, SQLITE3_INTEGER);
		if($q === false)
			return '?';
		$r=$q->execute();
		if($r === false)
			return '?';
		$r=$r->fetchArray();
		if($r === false)
			return '?';
		return $r['state'];
	}

	function setByDescription($description, $state){
		$q = $this->db->prepare('UPDATE light SET state=:state WHERE description=:description');
		$q->bindValue(':description', $description, SQLITE3_TEXT);
		$q->bindValue(':state', $state, SQLITE3_INTEGER);
		if($q === false)
			return '?';
		$r=$q->execute();
		if($r === false)
			return '?';
		return $this->getByDescription($description);
	}

	function setByChannel($channel, $state){
		$q = $this->db->prepare('UPDATE light SET state=:state WHERE channel=:channel');
		$q->bindValue(':channel', $channel, SQLITE3_INTEGER);
		$q->bindValue(':state', $state, SQLITE3_INTEGER);
		if($q === false)
			return '?';
		$r=$q->execute();
		if($r === false)
			return '?';
		return $this->getByChannel($channel);
	}

		

};

$l = new Light;
$r = explode('/', $_GET['request']);
switch($r[0]){
	case 'light.txt':
		echo '<text>'.$l->getAll().'</text>';
		break;
	case 'getAll':
		echo $l->getAll();
		break;
	case 'getIfAny':
		echo $l->getIfAny();
		break;
	case 'getByChannel':
		echo $l->getByChannel($r[1]);
		break;
	case 'getByDescription':
		echo $l->getByDescription($r[1]);
		break;
	case 'toggleByChannel':
		echo $l->toggleByChannel($r[1]);
		break;
	case 'toggleByDescription':
		echo $l->toggleByDescription($r[1]);
		break;
	case 'setByChannel':
		echo $l->setByChannel($r[1], $r[2]);
		break;
	case 'setByDescription':
		echo $l->setByDescription($r[1], $r[2]);
		break;
	case 'setAll':
		echo $l->setAll($r[1]);
		break;
	default:
		echo '?';
}
