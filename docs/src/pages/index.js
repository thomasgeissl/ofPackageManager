import React from "react"
import styled from "styled-components"
import Logo from "../assets/logo.png"
import { makeStyles } from "@material-ui/core/styles"
import Paper from "@material-ui/core/Paper"
import Grid from "@material-ui/core/Grid"

const StyledLogo = styled.img`
  width: 15%;
`
const StyledPaper = styled(Paper)`
  padding: 15px;
`
const PaperHeadline = styled.h3`
  margin-top: 0;
  font-size: 20px;
`

const Wrapper = styled.div`
  text-align: center;
  max-width: 1024px;
  margin: 0 auto;
  margin-bottom: 50px;
`
const Headline = styled.h1`
  font-size: 32px;
`
const SubHeadline = styled.h2`
  font-size: 28px;
  text-align: center;
  margin-top: 50px;
`
const Block = styled.div`
  text-align: left;
`

const CodeBlock = styled.div`
  background-color: rgb(64, 64, 64);
  color: white;
  padding: 15px;
  overflow-y: scroll;
  border-radius: 5px;
  margin-top: 5px;
  margin-bottom: 5px;
  margin-bottom: 15px;
  font-family: monaco;
`

export default () => (
  <Wrapper>
    <StyledLogo src={Logo}></StyledLogo>

    <Headline>
      ofPackageManager - a package manager for{" "}
      <a href="http://openframeworks.cc" target="_blank">
        openFrameworks
      </a>
    </Headline>
    <Block>
      <SubHeadline>How to install it?</SubHeadline>
      <Grid container spacing={3}>
        <Grid item xs={12}>
          <StyledPaper>
            <PaperHeadline>Windows</PaperHeadline>
            We are working on a{" "}
            <a href="https://chocolatey.org/" target="_blank">
              chocolatey
            </a>{" "}
            installable version. For now you can download the latest version
            manually from the{" "}
            <a
              href="https://github.com/thomasgeissl/ofPackageManager/releases"
              target="_blank"
            >
              github releases
            </a>{" "}
            page. You can add it to the path, so you do not have to type the
            full path.
          </StyledPaper>
        </Grid>
        <Grid item xs={12}>
          <StyledPaper>
            <PaperHeadline>OSX</PaperHeadline>
            You can either install ofPackageManager via brew or manually. It is
            recommended to use the brew package manager.<br></br>
            <h4>Brew</h4>
            <CodeBlock>
              brew tap thomasgeissl/tools<br></br>
              brew install ofpackagemanager<br></br>
            </CodeBlock>
            <h4>Manually</h4>
            <CodeBlock>
              bash -c "$(curl -sSL
              https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/scripts/osx/downloadAndAddToPath.sh)"
            </CodeBlock>
          </StyledPaper>
        </Grid>
        <Grid item xs={12}>
          <StyledPaper>
            <PaperHeadline>Linux</PaperHeadline>
            There is currently no compiled version for linux available. You will
            need to compile it yourself. The following line will download
            ofPackageManager, its dependencies and then compile it.
            <CodeBlock>
              sh -c "$(curl -sSL
              https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/scripts/install.sh)"
            </CodeBlock>
          </StyledPaper>
        </Grid>
      </Grid>
    </Block>
    <Block>
      <SubHeadline>How to use it?</SubHeadline>
      <Grid container spacing={3}>
        <Grid item xs={12}>
          <StyledPaper>
            First, the package manager needs to be configured globally. It will
            ask you where your openFramesworks installation is located and where
            to install the packages database.
            <CodeBlock>ofPackageManager config -g</CodeBlock>
          </StyledPaper>
        </Grid>
        <Grid item xs={12}>
          <StyledPaper>
            Then cd into your project directory. And you can install addons via
            id, github handle (github name / repository name) or a git url. It
            is recommended to install addons locally, which is the default.{" "}
            <br></br>
            <br></br>
            The package manager will also clone dependencies if specified in
            addons_config.mk.
            <CodeBlock>ofPackageManager install ofxMidi</CodeBlock>
          </StyledPaper>
        </Grid>
        <Grid item xs={12}>
          <StyledPaper>
            But you can also install them globally to your global openFrameworks
            addons directory.
            <CodeBlock>ofPackageManager install -g ofxMidi</CodeBlock>
          </StyledPaper>
        </Grid>
        <Grid item xs={12}>
          <StyledPaper>
            As mentioned above ofPackageManager also accepts commit hashes and
            tags to specify the version you wanna checkout.
            <CodeBlock>
              ofPackageManager install ofxMidi@1.1.1<br></br>
              ofPackageManager install
              ofxMidi@2f0e6343c817a4f5a33b60339c82b5d10be8af01
            </CodeBlock>
          </StyledPaper>
        </Grid>
        <Grid item xs={12}>
          <StyledPaper>
            If a package is not (yet) available in the database you can also
            install it via the github handle or its git url.
            <CodeBlock>
              ofPackageManager install kylemcdonald/ofxCv<br></br>
              ofPackageManager install https://github.com/npisanti/ofxPDSP.git
            </CodeBlock>
          </StyledPaper>
        </Grid>

        <Grid item xs={12}>
          <StyledPaper>
            To install all dependencies of an existing project simply call the
            install task without any further parameter. It will clone them and
            checkout the corresponding commits.
            <CodeBlock>ofPackageManager install</CodeBlock>
          </StyledPaper>
        </Grid>

        <Grid item xs={12}>
          <StyledPaper>
            The package manager can be also used to find available packages.
            <CodeBlock>
              ofPackageManager search ofxPDSP<br></br>
              ofPackageManager search github ofxAudioFile<br></br>
              ofPackageManager search github -u npisanti
            </CodeBlock>
          </StyledPaper>
        </Grid>

        <Grid item xs={12}>
          <StyledPaper>
            If you wanna add or update manually cloned depenencies then you need
            to call the add task.
            <CodeBlock>
              ofPackageManager add local_addons/ofxMidi <br></br>
              ofPackageManager add -A local_addons
            </CodeBlock>
          </StyledPaper>
        </Grid>
        <Grid item xs={12}>
          <StyledPaper>
            You can check if there are updates available and update the database
            with the following two commands.
            <CodeBlock>
              ofPackageManager doctor <br></br>
              ofPackageManager update
            </CodeBlock>
          </StyledPaper>
        </Grid>
      </Grid>
    </Block>
    <Block>
      <SubHeadline>How does it work?</SubHeadline>
      <Grid container spacing={3}>
        <Grid item xs={12}>
          <StyledPaper>
            For every dependency it will append the git url and commit hash to
            the corresponding line in your addons.make file.
            <CodeBlock>
              ofxUnitTests <br></br>
              local_addons/ofxCommandLineUtils#https://github.com/thomasgeissl/ofxCommandLineUtils.git@32edfb3ff{" "}
              <br></br>
              local_addons/ofxGit2#https://github.com/thomasgeissl/ofxGit2.git@e2b4da663
            </CodeBlock>
          </StyledPaper>
        </Grid>
      </Grid>
    </Block>
    <Block>
      <SubHeadline>How can I learn more about it?</SubHeadline>

      <Grid container spacing={3}>
        <Grid item xs={12}>
          <StyledPaper>
            Have a look at{" "}
            <a
              href="https://github.com/thomasgeissl/ofPackageManager"
              target="_blank"
            >
              ofPackageManager
            </a>
            , its implementation and issues on github. Or checkout the{" "}
            <a
              href="https://forum.openframeworks.cc/t/ofpackagemanager-pre-release-for-osx-testing/31565"
              target="_blank"
            >
              forum
            </a>{" "}
            and do not hesitate to ask questions.
          </StyledPaper>
        </Grid>
      </Grid>
    </Block>
  </Wrapper>
)
