<?php
class User {
    public function getName() {
        return $this->name;
    }
    private $name;
}
function greet($name) {
    echo "Hello $name";
}
?>
