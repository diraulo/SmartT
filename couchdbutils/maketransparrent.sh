out20121008122543.png  
out20121008123219.png  
out20121008133003.png
for foo in `ls out*.png`; do convert $foo  -transparent white $foo.png; done
