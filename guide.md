# an example of how to use covert to protect ssh keys
First you have to create a new group which will own the keys. I've set the
GID\_MIN to 6000 so the GID wouldn't conflict with any user groups.
    # groupadd dz_ssh -K GID_MIN=6000
Drop into a shell that's in your newly created group
    # su -g dz_ssh dzwdz

Then you need to create a directory for the keys. Remember to make it only
accessible by group members. Here's my setup:
    $ ll /var /var/covert /var/covert/ssh
    /var/:
    drwxrwx--x  3 admin admin 4096 Jun 12 16:26 covert
    [...] others omitted
    
    /var/covert/:
    d---rwx--- 2 admin dz_ssh 4096 Jun 12 16:31 ssh
    
    /var/covert/ssh:
    ----rwx--- 1 admin dz_ssh 399 Jun 12 16:31 id_ed25519
    ----rwx--- 1 admin dz_ssh  93 Jun 12 16:31 id_ed25519.pub

Generate the key (and copy the public key somewhere accessible)
    $ ssh-keygen -t ed25519 -f /var/covert/ssh/id_ed25519
    $ cp /var/covert/ssh/id_ed25519.pub ~

And lastly, add ssh-add to /etc/covert. In the example below, dzwdz is the group
which is privileged to use ssh-add as dz\_ssh.
    /bin/ssh-add	dzwdz	dz_ssh

Now it's time to test this. Login as your regular user and try this:
    dzwdz: ssh-add /var/covert/ssh/id_ed25519
    /var/covert/ssh/id_ed25519: Permission denied
    dzwdz: covert ssh-add /var/covert/ssh/id_ed25519
    Identity added: /var/covert/ssh/id_ed25519 (admin@vania)

Voila, now there's a slightly smaller risk of a malicious program stealing your
keys. Now you only have to worry about stuff with root access (so pretty much
every package install script). Also, `covert` is only 80 LoC, I'd encourage you
to audit it too - it's a new project, so I could've missed something.
