{
  "targets": [
    {
      "target_name": "GMC",
      "sources": [ "src/GMC.cc" ],
      "include_dirs": [
        "src"
      ],
      "link_settings": {
        "libraries": [
        ],
        "library_dirs": [
        ]
      },
      "msbuild_settings": {
        "Link": {
          "ImageHasSafeExceptionHandlers": "false"
        }
      }
    }
  ]
}
