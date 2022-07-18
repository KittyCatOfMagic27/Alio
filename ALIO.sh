function alio() {
  if [ $1 = "--build" ]
  then
    echo "Building..."
    g++ /home/$USER/ALIO/alio.cpp -o /home/$USER/ALIO/alio
    echo "Done Building!"
  else
    /home/$USER/ADPL/./alio "$@"
  fi
  return 69
}
