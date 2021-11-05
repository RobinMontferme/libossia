#!/bin/bash 
# Setup codesigning
# Thanks https://www.update.rocks/blog/osx-signing-with-travis/
(
  set +x
  KEY_CHAIN=build.keychain
  echo "$MAC_CERT_B64" | base64 --decode > ossia-cert.p12

  security create-keychain -p azure $KEY_CHAIN
  security default-keychain -s $KEY_CHAIN
  security unlock-keychain -p azure $KEY_CHAIN
  security import ossia-cert.p12 -k $KEY_CHAIN -P "$MAC_CERT_PASSWORD" -T /usr/bin/codesign;
  security set-key-partition-list -S apple-tool:,apple: -s -k azure $KEY_CHAIN

  rm -rf *.p12
)

codesign_osx() {
  local folder=${1}
  security unlock-keychain -p azure build.keychain
  find "$folder" -name '*.dylib' -exec codesign --force --timestamp --sign "ossia.io" {} \;
  find "$folder" -name '*.mxo' -exec codesign --force --timestamp --sign "ossia.io" {} \; # Max external
  find "$folder" -name '*.whl' -exec codesign --force --timestamp --sign "ossia.io" {} \; # python library
  find "$folder" -name '*.so' -exec codesign --force --timestamp --sign "ossia.io" {} \;
  find "$folder" -name '*.pd_darwin' -exec codesign --force --timestamp --sign "ossia.io" {} \; # pd external
  find "$folder" -name '*.d_amd64' -exec codesign --force --timestamp --sign "ossia.io" {} \; # pd external
}

notarize_osx() { 
  set +x 
  local zipfile=${1}
  local bundle_id=${2}
  xcrun altool --notarize-app \
      -t osx \
      -f "$zipfile" \
      --primary-bundle-id "$bundle_id" \
      -u jeanmichael.celerier@gmail.com \
      -p "@env:MAC_ALTOOL_PASSWORD"
}

release_macos_folder() {
  local folder="${1}"
  local zipfile="${2}"
  local bundle_id="${3}"

  codesign_osx "$folder"
  (
    cd "$folder"/.. || return
    ditto -c -k --sequesterRsrc --keepParent $(basename "$folder") "${ARTIFACTS_DIR}/$zipfile"
    notarize_osx "${ARTIFACTS_DIR}/$zipfile" "$bundle_id"
  )
}